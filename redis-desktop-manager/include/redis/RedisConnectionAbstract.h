#pragma once

#include <QObject>
#include <QQueue>
#include <QMap>

#include "RedisConnectionConfig.h"
#include "RedisException.h"
#include "Response.h"
#include "Command.h"

class TestRedisConnection;

class RedisConnectionAbstract : public QObject
{
    Q_OBJECT

    friend class TestRedisConnection;
public:

    RedisConnectionConfig config;    

    RedisConnectionAbstract(const RedisConnectionConfig & c);         
    virtual ~RedisConnectionAbstract() {};

    /** Interface for all connection classes **/

    virtual bool connect() = 0;

    virtual void disconnect() = 0;

    virtual QString getLastError() = 0;

    virtual QVariant execute(QString) = 0;

    virtual bool isConnected();

    virtual void runCommand(const Command &cmd) = 0;

    virtual Response getLastResponse();    

    /** 
     * Get list of databases with keys counters 
     * @emit databesesLoaded
     **/
    typedef QMap <QString, int> RedisDatabases;    

    /** 
     * Select db 
     * see  http://redis.io/commands/select 
     */
    void selectDb(int dbIndex);

    static RedisConnectionAbstract * createConnection(const RedisConnectionConfig & c);

public slots:
    void addCommand(const Command&);
    void getDatabases();

signals:
    void responseResived(const QVariant &, QObject *);
    void operationProgress(int percents, QObject *);
    void databesesLoaded(RedisConnectionAbstract::RedisDatabases);
    void errorOccurred(QString);

protected:
    bool connected;
    QTimer * executionTimer;
    Response resp;
    QByteArray readingBuffer;
    bool commandRunning;
    Command runningCommand;
    QQueue<Command> commands;

    virtual void init();

    virtual void sendResponse();

    void processCommandQueue();

protected slots:
    void executionTimeout();
};

Q_DECLARE_METATYPE(RedisConnectionAbstract::RedisDatabases)