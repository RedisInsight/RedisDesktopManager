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

void RedisClient::AbstractProtocol::getDatabases(std::function<void(DatabaseList)> callback)
{
    if (!m_connection->isConnected() && !m_connection->connect()) {
        throw ConnectionExeption("Cannot connect to host");
    }

    //  Get keys count
    Command cmd("info");
    Response result = CommandExecutor::execute(m_connection, cmd);

    DatabaseList availableDatabeses;

    if (result.isErrorMessage()) {
        return callback(availableDatabeses);
    }

    // Parse keyspace info
    QString keyspaceInfo = result.toString();
    QRegExp getDbAndKeysCount("(db\\d+):keys=(\\d+),expires=(\\d+)");
    int pos = 0;
    QString dbName;
    int keysCount;

    while ((pos = getDbAndKeysCount.indexIn(keyspaceInfo, pos)) != -1) {

        dbName = getDbAndKeysCount.cap(1);
        keysCount = getDbAndKeysCount.cap(2).toInt();
        availableDatabeses.insert(dbName, keysCount);

        pos += getDbAndKeysCount.matchedLength();
    }

    int dbCount = (dbName.isEmpty())? 0 : dbName.remove(0,2).toInt();

    //detect more db
    Response scanningResp;
    do {
        Command cmd(QString("select %1").arg(dbCount));
        scanningResp = CommandExecutor::execute(m_connection, cmd);
    } while (scanningResp.isOkMessage() && ++dbCount);

    // build db list
    for (int dbIndex = 0; dbIndex < dbCount; ++dbIndex)
    {
        dbName = QString("db%1").arg(dbIndex);

        if (availableDatabeses.contains(dbName))
            continue;

        availableDatabeses.insert(dbName, 0);
    }

    return callback(availableDatabeses);
}

void RedisClient::AbstractProtocol::getDatabaseKeys(uint dbIndex, std::function<void (const ConnectionsTree::Operations::RawKeysList &)> callback)
{
    auto keyCmd = RedisClient::Command("keys *", this, dbIndex);

    keyCmd.setCallBack(this, [this, callback](RedisClient::Response r) {
        qDebug() << "Keys response";
        callback(r.getValue().toStringList());
    });

    m_connection->runCommand(keyCmd);
}

QSharedPointer<ConnectionsTree::Operations::ConsoleOperations> RedisClient::AbstractProtocol::getConsoleOperations()
{
    return QSharedPointer<ConnectionsTree::Operations::ConsoleOperations>();
}

void RedisClient::AbstractProtocol::disconnect()
{

}

QString RedisClient::AbstractProtocol::getNamespaceSeparator()
{
    return m_connection->config.namespaceSeparator;
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

    Command cmd("info");
    Response info = CommandExecutor::execute(m_connection, cmd);

    if (info.isErrorMessage())
        return QStringList();

    return info.getValue().toStringList();

}
