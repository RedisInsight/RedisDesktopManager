#include "RedisConnectionAbstract.h"
#include "RedisConnection.h"
#include "RedisConnectionOverSsh.h"


RedisConnectionAbstract::RedisConnectionAbstract(const RedisConnectionConfig & c) 
    : config(c), connected(false), commandRunning(false) 
{
};

void RedisConnectionAbstract::init()
{
    executionTimer = new QTimer;
    executionTimer->setSingleShot(true);
    QObject::connect(executionTimer, SIGNAL(timeout()), this, SLOT(executionTimeout()));
}

RedisConnectionAbstract * RedisConnectionAbstract::createConnection(const RedisConnectionConfig & c)
{    
    if (c.useSshTunnel()) {
        return new RedisConnectionOverSsh(c);
    }
    else { 
        return new RedisConnection(c); 
    }

    return nullptr;
}

void RedisConnectionAbstract::getDatabases()
{
    if (!isConnected() && !connect()) {
        return;
    }

    RedisDatabases availableDatabeses;

    QVariant rawDbCount = execute("config get databases");

    QStringList dbInfo = rawDbCount.toStringList();

    int dbCount = 0;

    //redis >=2.6 & config commands allowed by configuration
    if (dbInfo.size() == 2) {
        dbCount = dbInfo.at(1).toInt();        
    } else { //cloud & legacy redis versions

        QString scanningResp;

        do {            
            scanningResp = execute(QString("select %1").arg(dbCount)).toString();    

        } while (scanningResp == "OK" && ++dbCount);
    }    

    // build db list
    for (int dbIndex = 0; dbIndex < dbCount; ++dbIndex)
    {
        availableDatabeses.insert(QString("db%1").arg(dbIndex), 0);
    }

    //    Get keys count        
    QVariant result = execute("INFO");

    if (result.isNull()) {
        emit databesesLoaded(availableDatabeses);
        return;
    }

    QStringList keyspaceInfo = result.toString().split("\r\n", QString::SkipEmptyParts);

    QRegExp getDbAndKeysCount("(db\\d+):keys=(\\d+),expires=(\\d+)");

    for (QString line : keyspaceInfo) {

        if (!line.contains(':')) // skip header  
            continue;

        int pos = getDbAndKeysCount.indexIn(line);

        if (pos == -1) //skip "bad" lines 
            continue;

        QString dbName = getDbAndKeysCount.cap(1);
        int keysCount = getDbAndKeysCount.cap(2).toInt();

        availableDatabeses[dbName] = keysCount;
    }

    emit databesesLoaded(availableDatabeses);
} 

bool RedisConnectionAbstract::selectDb(int dbIndex)
{
    return !execute(QString("select %1").arg(dbIndex)).isNull();
}

bool RedisConnectionAbstract::isConnected()
{
    return connected;
}

void RedisConnectionAbstract::processCommandQueue()
{
    if (commandRunning || commands.isEmpty()) {
        return;
    }

    runCommand(commands.dequeue());
}

void RedisConnectionAbstract::addCommand(const Command& cmd)
{
    commands.enqueue(cmd);

    processCommandQueue();
}

void RedisConnectionAbstract::sendResponse()
{
    executionTimer->stop();    

    if (runningCommand.hasCallback()) {
        
        QString callbackName = runningCommand.getCallbackName();
        
        QMetaObject::invokeMethod(
            runningCommand.getOwner(), callbackName.toUtf8().constData(),  
            Qt::AutoConnection, Q_ARG(Response, resp)
            );
        

    } else {
        emit responseResived(resp.getValue(), runningCommand.getOwner());
    }

    commandRunning = false;

    processCommandQueue();
}

Response RedisConnectionAbstract::getLastResponse()
{
    return resp;
}

void RedisConnectionAbstract::executionTimeout()
{
    if (!commandRunning) {
        return;
    }

    return sendResponse();    
}