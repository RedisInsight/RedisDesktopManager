#pragma once
#include <QSharedPointer>
#include <QString>
#include <QVariant>
#include <QByteArray>
#include <QPair>
#include "modules/value-editor/keymodel.h"
#include "modules/redisclient/redisclient.h"
#include "rowcache.h"

template < typename T > class KeyModel : public ValueEditor::Model
{  
public:
    KeyModel(QSharedPointer<RedisClient::Connection> connection,
             QString fullPath, int dbIndex, int ttl,
             bool isMultiRow,
             QString rowsCountCmd,
             QString partialLoadingCmd,
             QString fullLoadingCmd,
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

    virtual QString getKeyName() override
    {
        return m_keyFullPath;
    }

    virtual QString getKeyTitle() override
    {
        return QString("%1::db%2::%3")
                .arg(m_connection->getConfig().name())
                .arg(m_dbIndex).arg(m_keyFullPath);
    }

    virtual int getTTL() override
    {
        return m_ttl;
    }    

    virtual bool isPartialLoadingSupported() override
    {
        return m_connection->getServerVersion() >= 2.8;
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

    virtual void setKeyName(const QString& newKeyName) override
    {
        auto cmd = RedisClient::Command(m_dbIndex)
                << "RENAMENX" << m_keyFullPath << newKeyName;

        RedisClient::Response result;

        try {
            result = RedisClient::CommandExecutor::execute(m_connection, cmd);
        } catch (const RedisClient::CommandExecutor::Exception& e) {
            throw Exception("Connection error: " + QString(e.what()));
        }

        if (result.getValue().toInt() == 0) {
            throw Exception("Key with new name already exist in database");
        }

        m_keyFullPath = newKeyName;
    }

    virtual void setTTL(int) override
    {
        // TBD
    }

    virtual void removeKey() override
    {
        auto cmd = RedisClient::Command(m_dbIndex);
        cmd << "DEL" << m_keyFullPath << m_keyFullPath;

        RedisClient::Response result;

        try {
            result = RedisClient::CommandExecutor::execute(m_connection, cmd);
        } catch (const RedisClient::CommandExecutor::Exception& e) {
            throw Exception("Connection error: " + QString(e.what()));
        }

        m_isKeyRemoved = true;
        m_notifier->removed();
    }

    virtual void loadRows(unsigned long rowStart, unsigned long count, std::function<void(const QString&)> callback) override
    {
        if (isPartialLoadingSupported() && !m_fullLoadingCmdSupportsRanges) {
            QSharedPointer<RedisClient::ScanCommand> cmd(new RedisClient::ScanCommand(
                                                             m_partialLoadingCmd.arg(m_keyFullPath),
                                                             m_notifier.data(), m_dbIndex));
            try {
                m_connection->retrieveCollection(cmd, [this, callback, rowStart](QVariant result)
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
        } else {
            QVariantList rows;
            try {
                if (m_fullLoadingCmdSupportsRanges)
                    rows = getRowsRange(m_fullLoadingCmd, rowStart, count).toList();
                else
                    rows = getRowsRange(m_fullLoadingCmd).toList();
            } catch (const KeyModel::Exception& e) {
                callback(QString(e.what()));
            }
            try {
                addLoadedRowsToCache(rows, rowStart);
            } catch(const std::runtime_error& e) {
                callback(QString(e.what()));
            }
            callback(QString());
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

    int getRowCount(const QString &countCmd)
    {
        RedisClient::Command updateCmd(m_dbIndex);
        updateCmd << countCmd << m_keyFullPath;

        RedisClient::Response result;

        try {
            result = RedisClient::CommandExecutor::execute(m_connection, updateCmd);
        } catch (const RedisClient::CommandExecutor::Exception& e) {
            throw Exception("Connection error: " + QString(e.what()));
        }

        if (result.getType() == RedisClient::Response::Integer) {
            return result.getValue().toUInt();
        }

        return -1;
    }

    QVariant getRowsRange(const QString & baseCmd, unsigned long rowStart = 0, unsigned long count = 0)
    {
        QStringList cmd;

        if (rowStart == 0 && count == 0) {
            cmd << baseCmd << m_keyFullPath;
        } else {
            unsigned long rowEnd = std::min(m_rowCount, rowStart + count) - 1;

            if (baseCmd.contains(QChar(' '))) {
                QStringList suffixCmd(baseCmd.split(QChar(' ')));

                cmd << suffixCmd.at(0);
                suffixCmd.removeFirst();
                cmd << m_keyFullPath << QString::number(rowStart) << QString::number(rowEnd);
                cmd += suffixCmd;

            } else {
                cmd << baseCmd << m_keyFullPath << QString::number(rowStart) << QString::number(rowEnd);
            }
        }

        RedisClient::Command updateCmd(cmd, m_dbIndex);
        RedisClient::Response result;

        try {
            result = RedisClient::CommandExecutor::execute(m_connection, updateCmd);
        } catch (const RedisClient::CommandExecutor::Exception& e) {
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
    QString m_keyFullPath;
    int m_dbIndex;
    int m_ttl;
    bool m_isKeyRemoved;
    unsigned long m_rowCount;

    enum State { Initial, DataLoaded, Error };
    State m_currentState;

    bool m_isMultiRow;

    // CMD strings
    QString m_rowsCountCmd;
    QString m_partialLoadingCmd;
    QString m_fullLoadingCmd;
    bool m_fullLoadingCmdSupportsRanges;

    MappedCache<T> m_rowsCache;
    QSharedPointer<ValueEditor::ModelSignals> m_notifier;
};

inline QVariant valueToBinary(const QByteArray&value)
{
   QVariantList list;

   for(int index=0; index < value.length(); ++index) {
       list.append(QVariant((unsigned char)value.at(index)));
   }
   return QVariant(list);
}
