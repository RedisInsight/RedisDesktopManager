#include "abstractprotocol.h"
#include "commandexecutor.h"

RedisClient::AbstractProtocol::AbstractProtocol(RedisClient::Connection *connection)
    : m_connection(connection)
{
}

bool RedisClient::AbstractProtocol::auth()
{
    //todo: implement this
    // todo: emit signal about success connection to main Connection class
    // todo: check is socket succesufully connected before run this method in main connection class


    //if (config.useAuth()) {
    //    execute(QString("AUTH %1").arg(config.auth));
    //}

    //connected = (execute("PING") == "PONG");

   // if (!connected)
   //     emit errorOccurred("Redis server require password or password invalid");

    return true;
}

RedisClient::AbstractProtocol::DatabaseList RedisClient::AbstractProtocol::getDatabases()
{
    if (!m_connection->isConnected()) {
        throw ConnectionExeption("Connect to host before use operations");
    }

    int dbCount = 0;
    Response scanningResp;

    do {
        scanningResp = CommandExecutor::execute(m_connection, Command(QString("select %1").arg(dbCount)));
    } while (scanningResp.isOkMessage() && ++dbCount);

    // build db list
    DatabaseList availableDatabeses;
    for (int dbIndex = 0; dbIndex < dbCount; ++dbIndex)
    {
        availableDatabeses.insert(QString("db%1").arg(dbIndex), 0);
    }

    //  Get keys count
    Response result = CommandExecutor::execute(m_connection, Command("info"));

    if (result.isErrorMessage()) {
        return availableDatabeses;
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

    return availableDatabeses;
}

bool RedisClient::AbstractProtocol::selectDb(int index)
{
    Command cmd(QString("select %1").arg(index));
    Response result = CommandExecutor::execute(m_connection, cmd);
    return result.isOkMessage();
}
