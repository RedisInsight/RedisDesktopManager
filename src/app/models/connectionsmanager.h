#pragma once
#include <functional>
#include <QSharedPointer>
#include <qredisclient/connection.h>

#include "app/models/connectionconf.h"
#include "connections-tree/model.h"
#include "bulk-operations/connections.h"
#include "treeoperations.h"

namespace ValueEditor {
    class TabsModel;
}

class ConnectionsManager : public ConnectionsTree::Model, public BulkOperations::ConnectionsModel
{
    Q_OBJECT    

public:
    ConnectionsManager(const QString& m_configPath);

    ~ConnectionsManager(void);

    Q_INVOKABLE void addNewConnection(const ServerConfig& config, bool saveToConfig = true);

    Q_INVOKABLE void updateConnection(const ServerConfig& config);

    Q_INVOKABLE bool importConnections(const QString &);

    Q_INVOKABLE bool saveConnectionsConfigToFile(const QString&);

    Q_INVOKABLE bool testConnectionSettings(const ServerConfig& config);

    Q_INVOKABLE ServerConfig createEmptyConfig() const;

    void saveConfig();

    Q_INVOKABLE int size();

    // BulkOperations model methods
    QSharedPointer<RedisClient::Connection> getByIndex(int index) override;

    QStringList getConnections() override;

signals:
    void editConnection(ServerConfig config);

    void connectionAboutToBeEdited(QString name);

    void openConsole(QSharedPointer<RedisClient::Connection> connection);

    void openServerStats(QSharedPointer<RedisClient::Connection> connection);

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

     void createServerItemForConnection(QSharedPointer<RedisClient::Connection> connection,
                                        QSharedPointer<TreeOperations> treeModel);

private:
    QString m_configPath;    
    QList<QSharedPointer<RedisClient::Connection>> m_connections;
    QHash<QSharedPointer<RedisClient::Connection>,
          QSharedPointer<ConnectionsTree::TreeItem>> m_connectionMapping;

protected:
    bool loadConnectionsConfigFromFile(const QString& config, bool saveChangesToFile = false);
};

