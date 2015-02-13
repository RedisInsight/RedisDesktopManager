#include "treeoperations.h"
#include "redisclient/commandexecutor.h"
#include "redisclient/connection.h"
#include "redisclient/command.h"
#include "redisclient/scancommand.h"
#include "redisclient/response.h"
#include "app/widgets/consoletabs.h"
#include "console/consoletab.h"
#include "consoleoperations.h"
#include "value-editor/view.h"

TreeOperations::TreeOperations(QSharedPointer<RedisClient::Connection> connection, ConsoleTabs& tabs)
    : m_connection(connection), m_consoleTabs(tabs)
{

}

void TreeOperations::getDatabases(std::function<void (ConnectionsTree::Operations::DatabaseList)> callback)
{
    if (!m_connection->isConnected()) {

        bool result;

        try {
            result = m_connection->connect();
        } catch (const RedisClient::Connection::Exception& e) {
            throw ConnectionsTree::Operations::Exception("Cannot connect to host: " + QString(e.what()));
        }

        if (!result)
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
    if (m_connection->getServerVersion() >= 2.8) {
        QSharedPointer<RedisClient::ScanCommand> keyCmd(new RedisClient::ScanCommand("scan 0 MATCH *", this, dbIndex));
        m_connection->retrieveCollection(keyCmd, [this, callback](QVariant r) {

            if (r.type() == QMetaType::QVariantList)
                callback(r.toStringList());
            else
                callback(QStringList());
        });
    } else {
        auto keyCmd = RedisClient::Command("keys *", this, dbIndex);

        keyCmd.setCallBack(this, [this, callback](RedisClient::Response r) {
            callback(r.getValue().toStringList());
        });

        m_connection->runCommand(keyCmd);
    }
}

void TreeOperations::disconnect()
{
    m_connection->disconnect();
}

QString TreeOperations::getNamespaceSeparator()
{
    return m_connection->config.param<QString>("namespace_separator");
}

void TreeOperations::openKeyTab(ConnectionsTree::KeyItem& key, bool openInNewTab)
{
    emit openValueTab(m_connection, key, openInNewTab);
    qDebug() << "open key:" << key.getFullPath();
}

void TreeOperations::openConsoleTab()
{       
    QSharedPointer<ConsoleModel> model(new ConsoleModel(m_connection));
    QSharedPointer<Console::ConsoleTab> tab(new Console::ConsoleTab(model.staticCast<Console::Operations>()));
    m_consoleTabs.addTab(tab.staticCast<BaseTab>());
}

void TreeOperations::openNewKeyDialog(int dbIndex, QString keyPrefix)
{
    qDebug() << "new key";
    emit newKeyDialog(m_connection, dbIndex, keyPrefix);
}
