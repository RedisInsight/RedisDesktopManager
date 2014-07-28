#pragma once

#include <QSharedPointer>
#include "core/connection.h"
#include "connections-tree/model.h"

class RedisConnectionsManager : public ConnectionsTree::Model
{
    Q_OBJECT    

public:
    RedisConnectionsManager(QString);
    ~RedisConnectionsManager(void);

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

