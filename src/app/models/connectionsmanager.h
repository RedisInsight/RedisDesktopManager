#pragma once
#include <QSharedPointer>
#include <qredisclient/connection.h>
#include "app/models/connectionconf.h"
#include "connections-tree/model.h"
#include "treeoperations.h"

namespace ValueEditor {
    class ViewModel;
}

class ConnectionsManager : public ConnectionsTree::Model
{
    Q_OBJECT    

public:
    ConnectionsManager(const QString& m_configPath,
                       QSharedPointer<ValueEditor::ViewModel> values);

    ~ConnectionsManager(void);

    Q_INVOKABLE void addNewConnection(const ServerConfig& config, bool saveToConfig = true);

    Q_INVOKABLE void updateConnection(const ServerConfig& config);

    Q_INVOKABLE bool importConnections(const QString &);

    Q_INVOKABLE bool saveConnectionsConfigToFile(const QString&);

    Q_INVOKABLE bool testConnectionSettings(const ServerConfig& config);

    Q_INVOKABLE ServerConfig createEmptyConfig() const;

    void saveConfig();

    Q_INVOKABLE int size();

signals:
    void editConnection(ServerConfig config);

    void connectionAboutToBeEdited(QString name);

    void openConsole(QSharedPointer<RedisClient::Connection> connection);

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
    QSharedPointer<ValueEditor::ViewModel> m_valueTabs;

protected:
    bool loadConnectionsConfigFromFile(const QString& config, bool saveChangesToFile = false);
};

