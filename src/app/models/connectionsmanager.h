#pragma once

#include <QSharedPointer>
#include "redisclient/connection.h"
#include "redisclient/connectionconfig.h"
#include "connections-tree/model.h"

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

    void addNewConnection(const RedisClient::ConnectionConfig& config, bool saveToConfig = true);
    void updateConnection(const RedisClient::ConnectionConfig& config);

    bool importConnections(const QString &);
    bool saveConnectionsConfigToFile(const QString&);
    void saveConfig();
    int size();

signals:
    void editConnection(RedisClient::ConnectionConfig config);

private:
    QString m_configPath;    
    QList<QSharedPointer<RedisClient::Connection>> m_connections;
    QHash<QSharedPointer<RedisClient::Connection>,
          QSharedPointer<ConnectionsTree::TreeItem>> m_connectionMapping;
    ConsoleTabs& m_tabs;
    QSharedPointer<ValueEditor::ViewModel> m_values;    

protected:
    bool loadConnectionsConfigFromFile(const QString& config, bool saveChangesToFile = false);
};

