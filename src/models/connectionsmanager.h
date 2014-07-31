#pragma once

#include <QSharedPointer>
#include "core/connection.h"
#include "connections-tree/model.h"

class ConnectionsManager : public ConnectionsTree::Model
{
    Q_OBJECT    

public:
    ConnectionsManager(QString);
    ~ConnectionsManager(void);

    void AddConnection(QSharedPointer<RedisClient::Connection> connection);
    bool ImportConnections(QString &);
    bool SaveConnectionsConfigToFile(QString);

    int size();

private:
    QString configPath;
    bool connectionSettingsChanged;
    QList<QSharedPointer<RedisClient::Connection>> connections;

protected:
    bool LoadConnectionsConfigFromFile(QString& config, bool saveChangesToFile = false);    
};

