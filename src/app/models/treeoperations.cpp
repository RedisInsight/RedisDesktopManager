#include "treeoperations.h"
#include <qredisclient/redisclient.h>
#include "app/widgets/consoletabs.h"
#include "app/models/connectionconf.h"
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
    Response result;
    try {
        result = m_connection->commandSync("info");
    } catch (const RedisClient::Connection::Exception& e) {
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
            try {
                scanningResp = m_connection->commandSync("select", QString::number(dbCount));
            } catch (const RedisClient::Connection::Exception& e) {
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

void TreeOperations::getDatabaseKeys(uint dbIndex, std::function<void (const RawKeysList &, const QString &)> callback)
{
    QString keyPattern = static_cast<ConnectionConfig>(m_connection->getConfig()).keysPattern();

    if (m_connection->getServerVersion() >= 2.8) {
        QList<QByteArray> rawCmd {
            "scan", "0", "MATCH", keyPattern.toUtf8(), "COUNT", "10000"
        };
        QSharedPointer<RedisClient::ScanCommand> keyCmd(new RedisClient::ScanCommand(rawCmd, dbIndex));

        try {
            m_connection->retrieveCollection(keyCmd, [this, callback](QVariant r, QString err)
            {                
                if (!err.isEmpty())
                    callback(QStringList(), QString("Cannot load keys: %1").arg(err));

                callback(r.toStringList(), QString());
            });
        } catch (const RedisClient::Connection::Exception& error) {            
            callback(QStringList(), QString("Cannot load keys: %1").arg(error.what()));
        }
    } else {
        try {
            m_connection->command({"KEYS", keyPattern.toUtf8()}, this,
                                  [this, callback](RedisClient::Response r, QString)
            {
                callback(r.getValue().toStringList(), QString());
            }, dbIndex);
        } catch (const RedisClient::Connection::Exception& error) {
            callback(QStringList(), QString("Cannot load keys: %1").arg(error.what()));
        }
    }
}

void TreeOperations::disconnect()
{
    m_connection->disconnect();
}

QString TreeOperations::getNamespaceSeparator()
{
    return static_cast<ConnectionConfig>(m_connection->getConfig()).namespaceSeparator();
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
