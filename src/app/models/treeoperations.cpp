#include "treeoperations.h"
#include <QRegExp>
#include <qredisclient/redisclient.h>

#include "app/widgets/consoletabs.h"
#include "app/models/connectionconf.h"
#include "app/models/connectionsmanager.h"
#include "app/models/consoleoperations.h"

#include "console/consoletab.h"
#include "value-editor/view.h"
#include "connections-tree/items/namespaceitem.h"

#include <algorithm>

TreeOperations::TreeOperations(QSharedPointer<RedisClient::Connection> connection,
                               ConsoleTabs& tabs, ConnectionsManager &manager)
    : m_connection(connection),
      m_consoleTabs(tabs),
      m_manager(manager)
{
}

void TreeOperations::getDatabases(std::function<void (RedisClient::DatabaseList)> callback)
{
    if (!m_connection->isConnected()) {
        try {
            m_connection->connect(true);
        } catch (const RedisClient::Connection::Exception& e) {
            throw ConnectionsTree::Operations::Exception("Connection error: " + QString(e.what()));
        }
    }

    RedisClient::DatabaseList availableDatabeses = m_connection->getKeyspaceInfo();

    //detect all databases
    RedisClient::Response scanningResp;
    int dbIndex = (availableDatabeses.size() == 0)? 0 : availableDatabeses.lastKey() + 1;

    while (true) {
        try {
            scanningResp = m_connection->commandSync("select", QString::number(dbIndex));
        } catch (const RedisClient::Connection::Exception& e) {
            throw ConnectionsTree::Operations::Exception("Connection error: " + QString(e.what()));
        }

        if (!scanningResp.isOkMessage())
            break;

        availableDatabeses.insert(dbIndex, 0);
        ++dbIndex;
    }

    return callback(availableDatabeses);
}

void TreeOperations::getDatabaseKeys(uint dbIndex, std::function<void (const RedisClient::Connection::RawKeysList &, const QString &)> callback)
{
    QString keyPattern = static_cast<ConnectionConfig>(m_connection->getConfig()).keysPattern();

    try {
        m_connection->getDatabaseKeys(callback, keyPattern, dbIndex);
    } catch (const RedisClient::Connection::Exception& error) {
        callback(RedisClient::Connection::RawKeysList(), QString("Cannot load keys: %1").arg(error.what()));
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
    emit m_manager.openValueTab(m_connection, key, openInNewTab);
}

void TreeOperations::openConsoleTab()
{       
    QSharedPointer<ConsoleModel> model(new ConsoleModel(m_connection));
    QSharedPointer<Console::ConsoleTab> tab(new Console::ConsoleTab(model.staticCast<Console::Operations>()));
    m_consoleTabs.addTab(tab.staticCast<BaseTab>());
}

void TreeOperations::openNewKeyDialog(int dbIndex, std::function<void()> callback,
                                      QString keyPrefix)
{
    emit m_manager.newKeyDialog(m_connection, callback, dbIndex, keyPrefix);
}

void TreeOperations::notifyDbWasUnloaded(int dbIndex)
{
    emit m_manager.closeDbKeys(m_connection, dbIndex);
}

void TreeOperations::deleteDbKey(ConnectionsTree::KeyItem& key, std::function<void(const QString&)> callback)
{
    RedisClient::Command::Callback cmdCallback = [this, &key, &callback](const RedisClient::Response&, const QString& error)
    {
        if (!error.isEmpty()) {
          callback(QString("Cannot remove key: %1").arg(error));
          return;
        }

        QRegExp filter(key.getFullPath(), Qt::CaseSensitive, QRegExp::Wildcard);
        emit m_manager.closeDbKeys(m_connection, key.getDbIndex(), filter);
        key.setRemoved();
    };

    try {
        m_connection->command({"DEL", key.getFullPath()}, this, cmdCallback, key.getDbIndex());
    } catch (const RedisClient::Connection::Exception& e) {
        throw ConnectionsTree::Operations::Exception("Delete key error: " + QString(e.what()));
    }
}

void TreeOperations::deleteDbNamespace(ConnectionsTree::NamespaceItem &ns)
{
    QString pattern = QString("%1:*").arg(QString::fromUtf8(ns.getFullPath()));
    QRegExp filter(pattern, Qt::CaseSensitive, QRegExp::Wildcard);

    int dbIndex = ns.getDbIndex();

    emit m_manager.requestBulkOperation(m_connection, dbIndex,
                                        BulkOperations::Manager::Operation::DELETE_KEYS,
                                        filter, [this, dbIndex, filter, &ns]() {
        ns.setRemoved();
        emit m_manager.closeDbKeys(m_connection, dbIndex, filter);
    });
}

void TreeOperations::flushDb(int dbIndex, std::function<void(const QString&)> callback)
{
    RedisClient::Command::Callback cmdCallback = [this, callback](const RedisClient::Response&, const QString& error)
    {
        if (!error.isEmpty()) {
          callback(QString("Cannot remove key: %1").arg(error));
          return;
        }
        callback(QString());
    };

    try {
        m_connection->command({"FLUSHDB"}, this, cmdCallback, dbIndex);
    } catch (const RedisClient::Connection::Exception& e) {
        throw ConnectionsTree::Operations::Exception("FlushDB error: " + QString(e.what()));
    }
}
