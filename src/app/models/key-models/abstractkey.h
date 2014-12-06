#pragma once

#include <QSharedPointer>
#include <QString>
#include "modules/value-editor/keymodel.h"
#include "modules/redisclient/connection.h"

class KeyModel : public ValueEditor::Model
{
    Q_OBJECT    
public:
    KeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl);

    QString getKeyName() override;
    int getTTL() override;
    QString getState() override;

    bool isPartialLoadingSupported() override;
    unsigned long rowsCount() override;
    void setKeyName(const QString&) override;
    void setTTL(int) override;
    void removeKey() override;

protected:
    QSharedPointer<RedisClient::Connection> m_connection;
    QString m_keyFullPath;
    int m_dbIndex;
    int m_ttl;
    bool m_isKeyRemoved;
    unsigned long m_rowCount;

    enum State { Initial, DataLoaded, Error };
    State m_currentState;        

    //multi row internal operations
    int getRowCount(const QString &countCmd);    
    QVariant getRowsRange(const QString & baseCmd, unsigned long rowStart = -1, unsigned long count = -1);
};

class ListLikeKeyModel : public KeyModel
{
    Q_OBJECT
public:
    ListLikeKeyModel(QSharedPointer<RedisClient::Connection> connection,
                QString fullPath, int dbIndex, int ttl);


    QStringList getColumnNames() override;
    QHash<int, QByteArray> getRoles() override;
    QVariant getData(int rowIndex, int dataRole) override;

    void clearRowCache() override;
    bool isRowLoaded(int) override;
    bool isMultiRow() const override;

protected:
    enum Roles { Value = Qt::UserRole + 1, RowNumber};
    QHash<int, QByteArray> m_rowsCache;
};
