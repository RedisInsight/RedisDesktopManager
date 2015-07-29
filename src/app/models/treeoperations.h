#pragma once

#include <QSharedPointer>
#include <QObject>
#include "modules/redisclient/connection.h"
#include "modules/connections-tree/operations.h"
#include "modules/connections-tree/items/keyitem.h"


class ConsoleTabs;

class TreeOperations : public QObject, public ConnectionsTree::Operations
{
    Q_OBJECT
public:
    TreeOperations(QSharedPointer<RedisClient::Connection> connection,
                   ConsoleTabs& tabs);

    void getDatabases(std::function<void(DatabaseList)>) override;

    void getDatabaseKeys(uint dbIndex, std::function<void(const RawKeysList&)>) override;

    void disconnect() override;

    QString getNamespaceSeparator() override;    

    void openKeyTab(ConnectionsTree::KeyItem& key, bool openInNewTab = false) override;

    void openConsoleTab() override;

    void openNewKeyDialog(int dbIndex, QString keyPrefix = QString()) override;

    void notifyDbWasUnloaded(int dbIndex) override;

signals:
    void openValueTab(QSharedPointer<RedisClient::Connection> connection,
                      ConnectionsTree::KeyItem& key, bool inNewTab);

    void newKeyDialog(QSharedPointer<RedisClient::Connection> connection,
                      int dbIndex, QString keyPrefix);

    void closeDbKeys(QSharedPointer<RedisClient::Connection> connection, int dbIndex);

private:
     QSharedPointer<RedisClient::Connection> m_connection;
     ConsoleTabs& m_consoleTabs;
};
