#pragma once

#include <QtCore>
#include <QThread>
#include <QStandardItemModel>
#include "connection.h"

class TestRedisConnectionsManager;
class RedisServerItem;

class RedisConnectionsManager : public QStandardItemModel
{
    Q_OBJECT

    friend class TestRedisConnectionsManager;

public:
    RedisConnectionsManager(QString, QObject *);
    ~RedisConnectionsManager(void);

    void AddConnection(RedisClient::Connection *);
    bool RemoveConnection(RedisServerItem *);
    bool ImportConnections(QString &);
    bool SaveConnectionsConfigToFile(QString);

    void setFilter(QRegExp &);
    void resetFilter();

    void connectionChanged();

    int count();

private:
    QString configPath;
    bool connectionSettingsChanged;
    QThread connectionsThread;
    QList<RedisClient::Connection *> connections;
    QRegExp filter;

protected:
    bool LoadConnectionsConfigFromFile(QString& config, bool saveChangesToFile = false);    

protected slots:
    void updateFilter();
};

