#pragma once
#include <functional>
#include <QSharedPointer>
#include <QObject>
#include <qredisclient/connection.h>
#include "modules/connections-tree/operations.h"
#include "modules/connections-tree/items/keyitem.h"
#include "modules/bulk-operations/bulkoperationsmanager.h"

class ConsoleTabs;
class ConnectionsManager;

class TreeOperations : public QObject, public ConnectionsTree::Operations
{
    Q_OBJECT
public:
    TreeOperations(QSharedPointer<RedisClient::Connection> connection,
                   ConsoleTabs& tabs, ConnectionsManager& manager);

    void getDatabases(std::function<void(RedisClient::DatabaseList)>) override;

    void getDatabaseKeys(uint dbIndex, std::function<void(const RedisClient::Connection::RawKeysList&, const QString&)>) override;

    void disconnect() override;

    QString getNamespaceSeparator() override;    

    void openKeyTab(ConnectionsTree::KeyItem& key, bool openInNewTab = false) override;

    void openConsoleTab() override;

    void openNewKeyDialog(int dbIndex, std::function<void()> callback,
                          QString keyPrefix = QString()) override;

    void notifyDbWasUnloaded(int dbIndex) override;

    void deleteDbKey(ConnectionsTree::KeyItem& key, std::function<void(const QString&)> callback) override;

    void deleteDbNamespace(ConnectionsTree::NamespaceItem& ns) override;

    virtual void flushDb(int dbIndex, std::function<void(const QString&)> callback) override;

private:
     QSharedPointer<RedisClient::Connection> m_connection;
     ConsoleTabs& m_consoleTabs;
     ConnectionsManager& m_manager;
};
