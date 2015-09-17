#pragma once
#include <QSharedPointer>
#include <qredisclient/connection.h>
#include "app/models/connectionconf.h"
#include "connections-tree/model.h"
#include "treeoperations.h"

class ConsoleTabs;

namespace ValueEditor {
    class ViewModel;
}

class ConnectionsManager : public ConnectionsTree::Model
{
    Q_OBJECT    

public:
    ConnectionsManager(const QString& m_configPath, ConsoleTabs& tabs,
                       QSharedPointer<ValueEditor::ViewModel> values);
    ~ConnectionsManager(void);

    void addNewConnection(const ConnectionConfig& config, bool saveToConfig = true);
    void updateConnection(const ConnectionConfig& config);

    bool importConnections(const QString &);
    bool saveConnectionsConfigToFile(const QString&);
    void saveConfig();
    int size();

signals:
    void editConnection(ConnectionConfig config);

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
    QSharedPointer<ValueEditor::ViewModel> m_valueTabs;

protected:
    bool loadConnectionsConfigFromFile(const QString& config, bool saveChangesToFile = false);
};

