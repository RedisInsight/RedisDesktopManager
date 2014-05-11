#include "abstractprotocol.h"
#include "commandexecutor.h"
#include "connectionexception.h"

RedisClient::AbstractProtocol::AbstractProtocol(RedisClient::Connection *connection)
    : m_connection(connection)
{
}

void RedisClient::AbstractProtocol::auth()
{        
    // todo: check is socket succesufully connected before run this method in main connection class
    m_connection->m_connected = true;

    if (m_connection->config.useAuth()) {
        Command authCmd(QStringList() << "auth" << m_connection->config.auth);
        Response authResult = CommandExecutor::execute(m_connection, authCmd);
    }

    Command testCommand("ping");
    Response testResult = CommandExecutor::execute(m_connection, testCommand);

    if (testResult.toString() == "+PONG\r\n") {
        m_connection->setConnectedState();   
        emit authOk();
    } else {
        emit errorOccurred("Redis server require password or password invalid");
        m_connection->m_connected = false;
    }
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
    if (m_connection->m_dbNumber == index)
        return true;

    m_connection->m_dbNumber = index;
    QStringList commandParts;
    commandParts << "select" << QString::number(index);
    Command cmd(commandParts);
    Response result = CommandExecutor::execute(m_connection, cmd);
    return result.isOkMessage();
}

QStringList RedisClient::AbstractProtocol::getInfo()
{
    if (!m_connection->isConnected()) {
        throw ConnectionExeption("Connect to host before use operations");
    }

    Response info = CommandExecutor::execute(m_connection, Command("info"));

    if (info.isErrorMessage())
        return QStringList();

    return info.getValue().toStringList();

}
