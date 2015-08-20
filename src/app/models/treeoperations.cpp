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

#include <algorithm>

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
    Command cmd({QString("info")});

    Response result;
    try {
        result = CommandExecutor::execute(m_connection, cmd);
    } catch (const RedisClient::CommandExecutor::Exception& e) {
        throw ConnectionsTree::Operations::Exception("Connection error: " + QString(e.what()));
    }

    DatabaseList availableDatabeses;
    QSet<int> loadedDatabeses;

    if (result.isErrorMessage()) {
        return callback(availableDatabeses);
    }

    // Parse keyspace info
    QString keyspaceInfo = result.getValue().toString();
    QRegularExpression getDbAndKeysCount("^db(\\d+):keys=(\\d+)");
    getDbAndKeysCount.setPatternOptions(QRegularExpression::MultilineOption);
    QRegularExpressionMatchIterator iter = getDbAndKeysCount.globalMatch(keyspaceInfo);
    while (iter.hasNext()) {
        QRegularExpressionMatch match = iter.next();
        int dbIndex = match.captured(1).toInt();
        availableDatabeses.push_back({dbIndex, match.captured(2).toInt()});
        loadedDatabeses.insert(dbIndex);
    }

    int dbCount = (loadedDatabeses.isEmpty())? 0 : *std::max_element(loadedDatabeses.begin(),
                                                                     loadedDatabeses.end());
    //detect more db if needed
    if (dbCount == 0) {
        Response scanningResp;
        do {
            Command cmd({"select", QString::number(dbCount)});
            try {
                scanningResp = CommandExecutor::execute(m_connection, cmd);
            } catch (const RedisClient::CommandExecutor::Exception& e) {
                throw ConnectionsTree::Operations::Exception("Connection error: " + QString(e.what()));
            }
        } while (scanningResp.isOkMessage() && ++dbCount);
    }

    // build db list
    for (int dbIndex = 0; dbIndex < dbCount; ++dbIndex)
    {
        if (loadedDatabeses.contains(dbIndex))
            continue;
        availableDatabeses.push_back({dbIndex, 0});
    }

    std::sort(availableDatabeses.begin(), availableDatabeses.end(),
              [](QPair<int, int> l, QPair<int, int> r) {
        return l.first < r.first;
    });

    return callback(availableDatabeses);
}

void TreeOperations::getDatabaseKeys(uint dbIndex, std::function<void (const ConnectionsTree::Operations::RawKeysList &)> callback)
{
    QString keyPattern = m_connection->getConfig().keysPattern();

    if (m_connection->getServerVersion() >= 2.8) {        
        QSharedPointer<RedisClient::ScanCommand> keyCmd(
                    new RedisClient::ScanCommand({"scan", "0", "MATCH", keyPattern, "COUNT", "10000"}, this, dbIndex));

        try {
            m_connection->retrieveCollection(keyCmd, [this, callback](QVariant r)
            {
                if (r.type() == QVariant::Type::List)
                    callback(r.toStringList());
                else
                    callback(QStringList());
            });
        } catch (const RedisClient::Connection::Exception& error) {
            qDebug() << "Cannot load keys:" << QString(error.what());
            //TODO: return callback with error
        }
    } else {
        auto keyCmd = RedisClient::Command({"KEYS", keyPattern}, this, dbIndex);

        keyCmd.setCallBack(this, [this, callback](RedisClient::Response r) {
            callback(r.getValue().toStringList());
        });

        try {
            m_connection->runCommand(keyCmd);
        } catch (const RedisClient::Connection::Exception& error) {
            qDebug() << "Cannot load keys:" << QString(error.what());
            //TODO: return callback with error
        }
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
}

void TreeOperations::openConsoleTab()
{       
    QSharedPointer<ConsoleModel> model(new ConsoleModel(m_connection));
    QSharedPointer<Console::ConsoleTab> tab(new Console::ConsoleTab(model.staticCast<Console::Operations>()));
    m_consoleTabs.addTab(tab.staticCast<BaseTab>());
}

void TreeOperations::openNewKeyDialog(int dbIndex, QString keyPrefix)
{
    emit newKeyDialog(m_connection, dbIndex, keyPrefix);
}

void TreeOperations::notifyDbWasUnloaded(int dbIndex)
{
    emit closeDbKeys(m_connection, dbIndex);
}
