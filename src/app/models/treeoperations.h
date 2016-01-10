#pragma once
#include <functional>
#include <QSharedPointer>
#include <QObject>
#include <qredisclient/connection.h>
#include "modules/connections-tree/operations.h"
#include "modules/connections-tree/items/keyitem.h"



class TreeOperations : public QObject, public ConnectionsTree::Operations
{
    Q_OBJECT
public:
    TreeOperations(QSharedPointer<RedisClient::Connection> connection);

    void getDatabases(std::function<void(DatabaseList)>) override;

    void getDatabaseKeys(uint dbIndex, std::function<void(const RawKeysList&, const QString&)>) override;

    void disconnect() override;

    QString getNamespaceSeparator() override;    

    void openKeyTab(ConnectionsTree::KeyItem& key, bool openInNewTab = false) override;

    void openConsoleTab() override;

    void openNewKeyDialog(int dbIndex, std::function<void()> callback,
                          QString keyPrefix = QString()) override;

    void notifyDbWasUnloaded(int dbIndex) override;

signals:
    void openValueTab(QSharedPointer<RedisClient::Connection> connection,
                      ConnectionsTree::KeyItem& key, bool inNewTab);

    void openConsole(QSharedPointer<RedisClient::Connection> connection);

    void newKeyDialog(QSharedPointer<RedisClient::Connection> connection,
                      std::function<void()> callback,
                      int dbIndex, QString keyPrefix);

    void closeDbKeys(QSharedPointer<RedisClient::Connection> connection, int dbIndex);

private:
     QSharedPointer<RedisClient::Connection> m_connection;     
};
