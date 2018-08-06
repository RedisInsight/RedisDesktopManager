#pragma once
#include <functional>
#include <QSharedPointer>
#include <QObject>
#include <QEnableSharedFromThis>
#include "modules/connections-tree/operations.h"
#include "modules/connections-tree/items/keyitem.h"
#include "modules/bulk-operations/bulkoperationsmanager.h"


class ConnectionsManager;


class TreeOperations : public QObject, public ConnectionsTree::Operations,
        public QEnableSharedFromThis<TreeOperations>
{
    Q_OBJECT
public:
    TreeOperations(QSharedPointer<RedisClient::Connection> connection, ConnectionsManager& manager);

    void getDatabases(std::function<void(RedisClient::DatabaseList)>) override;

    void loadNamespaceItems(QSharedPointer<ConnectionsTree::AbstractNamespaceItem> parent,
                            const QString& filter,
                            std::function<void(const QString& err)> callback) override;

    void disconnect() override;

    QString getNamespaceSeparator() override;    

    QString defaultFilter() override;

    void openKeyTab(ConnectionsTree::KeyItem& key, bool openInNewTab = false) override;

    void openConsoleTab() override;

    void openNewKeyDialog(int dbIndex, std::function<void()> callback,
                          QString keyPrefix = QString()) override;

    void openServerStats() override;

    void notifyDbWasUnloaded(int dbIndex) override;

    void deleteDbKey(ConnectionsTree::KeyItem& key, std::function<void(const QString&)> callback) override;

    void deleteDbNamespace(ConnectionsTree::NamespaceItem& ns) override;

    virtual void flushDb(int dbIndex, std::function<void(const QString&)> callback) override;

    virtual QString mode() override;

    void setConnection(QSharedPointer<RedisClient::Connection> c);

private:
     QSharedPointer<RedisClient::Connection> m_connection;
     ConnectionsManager& m_manager;
     uint m_dbCount;
};
