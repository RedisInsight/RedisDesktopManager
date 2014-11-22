#pragma once

#include <QSharedPointer>
#include "redisclient/connection.h"
#include "connections-tree/model.h"

class MainTabsWidget;

class ConnectionsManager : public ConnectionsTree::Model
{
    Q_OBJECT    

public:
    ConnectionsManager(const QString& configPath, MainTabsWidget& tabs);
    ~ConnectionsManager(void);

    void addConnection(QSharedPointer<RedisClient::Connection> connection);
    bool importConnections(const QString &);
    bool saveConnectionsConfigToFile(const QString&);

    int size();

private:
    QString configPath;
    bool connectionSettingsChanged;
    QList<QSharedPointer<RedisClient::Connection>> connections;
    MainTabsWidget& m_tabs;

protected:
    bool loadConnectionsConfigFromFile(const QString& config, bool saveChangesToFile = false);
};

