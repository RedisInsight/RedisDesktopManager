#pragma once

#include <QObject>
#include "RedisConnectionAbstract.h"

class RedisConnectionConfig;
class Command;

class ConnectionBridge : public QObject
{
    Q_OBJECT

public:
    ConnectionBridge(const RedisConnectionConfig & c);
    ~ConnectionBridge(void);

    void initWorker();

    void addCommand(const Command & cmd);

    QString getLastError();

    RedisConnectionConfig getConfig();    

    void setConnectionConfig(RedisConnectionConfig&);

signals:
    void responseResieved(const QVariant&, QObject *);
    void addCommandToWorker(const Command &);
    void loadDatabasesList();
    void dbListLoaded(RedisConnectionAbstract::RedisDatabases);
    void error(QString);
    void log(QString);
    void operationProgress(int percents, QObject *);

protected:
    RedisConnectionConfig config;

private:
    QThread * workerThread;
    RedisConnectionAbstract * worker;
    bool isInitialized;

    void stopWorker();    
};

