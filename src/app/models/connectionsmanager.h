#pragma once

#include <QSharedPointer>
#include "redisclient/connection.h"
#include "connections-tree/model.h"

class ConsoleTabs;

class ConnectionsManager : public ConnectionsTree::Model
{
    Q_OBJECT    

public:
    ConnectionsManager(const QString& configPath, ConsoleTabs& tabs);
    ~ConnectionsManager(void);

    void AddConnection(QSharedPointer<RedisClient::Connection> connection);
    bool ImportConnections(const QString &);
    bool SaveConnectionsConfigToFile(const QString&);

    int size();

private:
    QString configPath;
    bool connectionSettingsChanged;
    QList<QSharedPointer<RedisClient::Connection>> connections;
    ConsoleTabs& m_tabs;

protected:
    bool LoadConnectionsConfigFromFile(const QString& config, bool saveChangesToFile = false);
};

