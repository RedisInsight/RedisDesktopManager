#pragma once

#include "ItemWithNaturalSort.h"
#include "connection.h"
#include "defaultprotocol.h"
#include "RedisServerDbItem.h"

class RedisKeyItem;
class RedisConnectionsManager;

class RedisServerItem : public QObject, public ItemWithNaturalSort
{
    Q_OBJECT

    friend class RedisServerDbItem;
    friend class RedisKeyItem;

public:    
    RedisServerItem(RedisClient::Connection * c);

    void loadDatabaseList();
    QStringList getInfo();

    void reload();
    void unload();

    RedisClient::Connection * getConnection();
    void setConnection(RedisClient::Connection * c);

    bool isLocked();

    int virtual type() const;
    
    const static int TYPE = 2000;

private:    
    RedisClient::Connection * connection;
    bool isDbInfoLoaded;
    bool locked;

    void setBusyIcon();
    void setOfflineIcon();
    void setNormalIcon();
    void getItemNameFromConnection();

private slots:    
    void proccessError(QString);

signals:
    void error(QString);
    void statusMessage(QString);
    void databasesLoaded();
    void unlockUI();
};

