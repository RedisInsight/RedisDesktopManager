#include "treeoperations.h"
#include "redisclient/commandexecutor.h"
#include "redisclient/connection.h"
#include "redisclient/command.h"
#include "redisclient/response.h"

TreeOperations::TreeOperations(QSharedPointer<RedisClient::Connection> connection)
    : m_connection(connection)
{

}

void TreeOperations::getDatabases(std::function<void (ConnectionsTree::Operations::DatabaseList)> callback)
{
    if (!m_connection->isConnected() && !m_connection->connect()) {
        throw ConnectionsTree::Operations::Exception("Cannot connect to host");
    }

    using namespace RedisClient;

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

void TreeOperations::getDatabaseKeys(uint dbIndex, std::function<void (const ConnectionsTree::Operations::RawKeysList &)> callback)
{
    auto keyCmd = RedisClient::Command("keys *", this, dbIndex);

    keyCmd.setCallBack(this, [this, callback](RedisClient::Response r) {
        qDebug() << "Keys response";
        callback(r.getValue().toStringList());
    });

    m_connection->runCommand(keyCmd);
}

void TreeOperations::disconnect()
{
    //TBD
}

QString TreeOperations::getNamespaceSeparator()
{
    return m_connection->config.namespaceSeparator;
}

void TreeOperations::openKeyTab()
{

}

void TreeOperations::openNewKeyTab()
{

}

void TreeOperations::openConsoleTab()
{

}
