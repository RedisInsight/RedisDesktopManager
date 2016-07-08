#pragma once
#include <QSharedPointer>
#include <qredisclient/connection.h>
#include "app/models/connectionconf.h"
#include "connections-tree/model.h"
#include "bulk-operations/connections.h"
#include "treeoperations.h"

class ConsoleTabs;

class ConnectionsManager : public ConnectionsTree::Model, public BulkOperations::ConnectionsModel
{
    Q_OBJECT    

public:
    ConnectionsManager(const QString& m_configPath, ConsoleTabs& tabs);
    ~ConnectionsManager(void);

    void addNewConnection(const ConnectionConfig& config, bool saveToConfig = true);
    void updateConnection(const ConnectionConfig& config);

    bool importConnections(const QString &);
    bool saveConnectionsConfigToFile(const QString&);
    void saveConfig();
    int size();

    // BulkOperations model methods
    QSharedPointer<RedisClient::Connection> getByIndex(int index) override;

    QStringList getConnections() override;

signals:
    void editConnection(ConnectionConfig config);

    // Proxy-signals from TreeOperationsModel
    void openValueTab(QSharedPointer<RedisClient::Connection> connection,
                      ConnectionsTree::KeyItem& key, bool inNewTab);

    void newKeyDialog(QSharedPointer<RedisClient::Connection> connection,
                      std::function<void()> callback,
                      int dbIndex, QString keyPrefix);

    void closeDbKeys(QSharedPointer<RedisClient::Connection> connection, int dbIndex,
                     const QRegExp& filter=QRegExp("*", Qt::CaseSensitive, QRegExp::Wildcard));

    void requestBulkOperation(QSharedPointer<RedisClient::Connection> connection,
                              int dbIndex, BulkOperations::Manager::Operation op,
                              QRegExp keyPattern, std::function<void()> callback);

private:
     QSharedPointer<TreeOperations> createTreeModelForConnection(QSharedPointer<RedisClient::Connection> connection);
     void registerLogger(QSharedPointer<RedisClient::Connection> connection);
     void createServerItemForConnection(QSharedPointer<RedisClient::Connection> connection,
                                        QSharedPointer<TreeOperations> treeModel);


private:
    QString m_configPath;    
    QList<QSharedPointer<RedisClient::Connection>> m_connections;
    QHash<QSharedPointer<RedisClient::Connection>,
          QSharedPointer<ConnectionsTree::TreeItem>> m_connectionMapping;
    ConsoleTabs& m_consoleTabs;       

protected:
    bool loadConnectionsConfigFromFile(const QString& config, bool saveChangesToFile = false);
};

