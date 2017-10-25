#pragma once
#include <QSharedPointer>
#include <QString>
#include <QVariant>
#include <QByteArray>
#include <QPair>
#include <QDebug>
#include <qredisclient/redisclient.h>
#include <qredisclient/utils/text.h>
#include "modules/value-editor/keymodel.h"
#include "rowcache.h"

template < typename T > class KeyModel : public ValueEditor::Model
{  
public:
    KeyModel(QSharedPointer<RedisClient::Connection> connection,
             QByteArray fullPath, int dbIndex, int ttl,
             bool isMultiRow,
             QByteArray rowsCountCmd,
             QByteArray partialLoadingCmd,
             QByteArray fullLoadingCmd=QByteArray(),
             bool fullLoadingCmdSupportsRanges=false)
        :
          m_connection(connection),
          m_keyFullPath(fullPath),
          m_dbIndex(dbIndex),
          m_ttl(ttl),
          m_isKeyRemoved(false),
          m_rowCount(-1),
          m_currentState(KeyModel::State::Initial),
          m_isMultiRow(isMultiRow),
          m_rowsCountCmd(rowsCountCmd),
          m_partialLoadingCmd(partialLoadingCmd),
          m_fullLoadingCmd(fullLoadingCmd),
          m_fullLoadingCmdSupportsRanges(fullLoadingCmdSupportsRanges),
          m_notifier(new ValueEditor::ModelSignals())
    {
        try {
            loadRowsCount();
        } catch (const ValueEditor::Model::Exception& e) {
            qDebug() << "Connection error:" << e.what(); // TODO(u_glide): Notify user about error
        }
    }

    virtual ~KeyModel()
    {
        qDebug() << "{!!DEL!!} Remove keymodel";
        m_notifier.clear();
    }

    virtual QString getKeyName() override
    {
        return printableString(m_keyFullPath);
    }

    virtual QString getKeyTitle() override
    {
        return QString("%1::db%2::%3")
                .arg(m_connection->getConfig().name())
                .arg(m_dbIndex).arg(getKeyName());
    }

    virtual long long getTTL() override
    {
        return m_ttl;
    }

    virtual bool isMultiRow() const override
    {
        return m_isMultiRow;
    }

    virtual bool isRowLoaded(int rowIndex) override
    {
        return m_rowsCache.isRowLoaded(rowIndex);
    }

    virtual unsigned long rowsCount() override
    {
       return m_rowCount;
    }

    virtual void setKeyName(const QByteArray& newKeyName) override
    {
        RedisClient::Response result;

        try {
            result = m_connection->commandSync({"RENAMENX", m_keyFullPath, newKeyName}, m_dbIndex);
        } catch (const RedisClient::Connection::Exception& e) {
            throw Exception("Connection error: " + QString(e.what()));
        }

        if (result.getValue().toInt() == 0) {
            throw Exception("Key with new name already exist in database");
        }

        m_keyFullPath = newKeyName;
    }

    virtual void setTTL(const long long ttl) override
    {
        RedisClient::Response result;
        qDebug() << QString("TTL=%1").arg(ttl);
        try {
            if (ttl >= 0)
                result = m_connection->commandSync({"EXPIRE", m_keyFullPath, QString::number(ttl).toLatin1()}, m_dbIndex);
            else
                result = m_connection->commandSync({"PERSIST", m_keyFullPath}, m_dbIndex);
        } catch (const RedisClient::Connection::Exception& e) {
            throw Exception("Connection error: " + QString(e.what()));
        }

        if (result.getValue().toInt() == 0) {
            throw Exception("Not support TTL at this key");
        }
        if (ttl >= 0)
            m_ttl = ttl;
        else
            m_ttl = -1;
    }

    virtual void removeKey() override
    {
        RedisClient::Response result;

        try {
            result = m_connection->commandSync({"DEL", m_keyFullPath}, m_dbIndex);
        } catch (const RedisClient::Connection::Exception& e) {
            throw Exception("Connection error: " + QString(e.what()));
        }

        m_isKeyRemoved = true;
        m_notifier->removed();
    }

    virtual void loadRows(unsigned long rowStart, unsigned long count, std::function<void(const QString&)> callback) override
    {
        if (m_fullLoadingCmdSupportsRanges) {
            QVariantList rows;
            try {                
                rows = getRowsRange(m_fullLoadingCmd, rowStart, count).toList();
            } catch (const KeyModel::Exception& e) {
                callback(QString(e.what()));
            }
            try {
                addLoadedRowsToCache(rows, rowStart);
            } catch(const std::runtime_error& e) {
                callback(QString(e.what()));
            }
            callback(QString());
        } else {
            QList<QByteArray> cmdParts = m_partialLoadingCmd.split(' ');
            cmdParts.replace(cmdParts.indexOf("%1"), m_keyFullPath);

            RedisClient::ScanCommand cmd(cmdParts, m_dbIndex);
            try {
                m_connection->retrieveCollection(cmd, [this, callback, rowStart](QVariant result, QString)
                {
                    if (result.type() == QVariant::Type::List) {
                        try {
                            addLoadedRowsToCache(result.toList(), rowStart);
                        } catch(const std::runtime_error& e) {
                            callback(QString(e.what()));
                        }
                    }
                    callback(QString());
                });
            } catch (const RedisClient::Connection::Exception& e) {
                callback(QString(e.what()));
            }
        }
    }

    virtual void clearRowCache() override
    {
        m_rowsCache.clear();
    }

    virtual QSharedPointer<ValueEditor::ModelSignals> getConnector() const override
    {
        return m_notifier;
    }

    virtual QSharedPointer<RedisClient::Connection> getConnection() const override
    {
        return m_connection;
    }

    virtual unsigned int dbIndex() const override
    {
        return m_dbIndex;
    }

protected:
    //multi row internal operations
    void loadRowsCount()
    {
        if (isMultiRow()) {
            m_rowCount = getRowCount(m_rowsCountCmd);
        } else {
            m_rowCount = 1;
        }
    }

    int getRowCount(const QByteArray &countCmd)
    {
        RedisClient::Response result;

        try {
            result = m_connection->commandSync({countCmd, m_keyFullPath}, m_dbIndex);
        } catch (const RedisClient::Connection::Exception& e) {
            throw Exception("Connection error: " + QString(e.what()));
        }

        if (result.getType() == RedisClient::Response::Integer) {
            return result.getValue().toUInt();
        }

        return -1;
    }

    QVariant getRowsRange(const QByteArray & baseCmd, unsigned long rowStart, unsigned long count)
    {
        QList<QByteArray> cmd;

        unsigned long rowEnd = std::min(m_rowCount, rowStart + count) - 1;

        if (baseCmd.contains(' ')) {
            QList<QByteArray> suffixCmd(baseCmd.split(' '));

            cmd << suffixCmd.takeFirst();
            cmd << m_keyFullPath
                << QString::number(rowStart).toLatin1()
                << QString::number(rowEnd).toLatin1();
            cmd += suffixCmd;

        } else {
            cmd << baseCmd
                << m_keyFullPath
                << QString::number(rowStart).toLatin1()
                << QString::number(rowEnd).toLatin1();
        }

        RedisClient::Response result;

        try {
            result = m_connection->commandSync(cmd, m_dbIndex);
        } catch (const RedisClient::Connection::Exception& e) {
            throw Exception("Connection error: " + QString(e.what()));
        }

        if (result.getType() != RedisClient::Response::MultiBulk) {
            throw Exception("getRowsRange() error - can't load values from server");
        }

        return result.getValue();
    }

    // row validator
    virtual bool isRowValid(const QVariantMap &row)
    {
        if (row.isEmpty())
            return false;

        QSet<QString> validKeys;

        foreach (QByteArray role, getRoles().values()) {
            validKeys.insert(role);
        }

        QMapIterator<QString, QVariant> i(row);

        while (i.hasNext()) {
            i.next();

            if(!validKeys.contains(i.key()))
                return false;
        }

        return true;
    }

    virtual void setRemovedIfEmpty()
    {
        if (m_rowCount == 0) {
            m_isKeyRemoved = true;
            m_notifier->removed();
        }
    }

    virtual void addLoadedRowsToCache(const QVariantList& rows, int rowStart) = 0;

protected:
    QSharedPointer<RedisClient::Connection> m_connection;
    QByteArray m_keyFullPath;
    int m_dbIndex;
    long long m_ttl;
    bool m_isKeyRemoved;
    unsigned long m_rowCount;

    enum State { Initial, DataLoaded, Error };
    State m_currentState;

    bool m_isMultiRow;

    // CMD strings
    QByteArray m_rowsCountCmd;
    QByteArray m_partialLoadingCmd;
    QByteArray m_fullLoadingCmd;
    bool m_fullLoadingCmdSupportsRanges;

    MappedCache<T> m_rowsCache;
    QSharedPointer<ValueEditor::ModelSignals> m_notifier;
};

