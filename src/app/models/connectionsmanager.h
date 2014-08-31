#pragma once

#include <QSharedPointer>
#include "redisclient/connection.h"
#include "connections-tree/model.h"

class ConnectionsManager : public ConnectionsTree::Model
{
    Q_OBJECT    

public:
    ConnectionsManager(const QString& configPath);
    ~ConnectionsManager(void);

    void AddConnection(QSharedPointer<RedisClient::Connection> connection);
    bool ImportConnections(const QString &);
    bool SaveConnectionsConfigToFile(const QString&);

    int size();

private:
    QString configPath;
    bool connectionSettingsChanged;
    QList<QSharedPointer<RedisClient::Connection>> connections;

protected:
    bool LoadConnectionsConfigFromFile(const QString& config, bool saveChangesToFile = false);
};

