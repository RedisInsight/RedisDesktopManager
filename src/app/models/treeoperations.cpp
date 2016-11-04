#include "treeoperations.h"

#include <QRegExp>
#include <algorithm>
#include <QSet>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <qredisclient/redisclient.h>

#include "app/models/connectionconf.h"
#include "app/models/connectionsmanager.h"
#include "connections-tree/items/namespaceitem.h"


TreeOperations::TreeOperations(QSharedPointer<RedisClient::Connection> connection, ConnectionsManager &manager)
    : m_connection(connection),
      m_manager(manager)
{
}

void TreeOperations::getDatabases(std::function<void (RedisClient::DatabaseList)> callback)
{
    if (!m_connection->isConnected()) {
        try {
            m_connection->connect(true);
        } catch (const RedisClient::Connection::Exception& e) {
            throw ConnectionsTree::Operations::Exception(QObject::tr("Connection error: ") + QString(e.what()));
        }
    }

    if (m_connection->getServerVersion() < 2.8)
        throw ConnectionsTree::Operations::Exception(QObject::tr("RedisDesktopManager >= 0.9.0 doesn't support old versions of "
                                                     "redis-server (< 2.8). Please use RedisDesktopManager 0.8.8 or upgrade your redis-server."));

    RedisClient::DatabaseList availableDatabeses = m_connection->getKeyspaceInfo();

    if (m_connection->mode() != RedisClient::Connection::Mode::Cluster) {
        //detect all databases
        RedisClient::Response scanningResp;
        int dbIndex = (availableDatabeses.size() == 0)? 0 : availableDatabeses.lastKey() + 1;

        while (true) {
            try {
                scanningResp = m_connection->commandSync("select", QString::number(dbIndex));
            } catch (const RedisClient::Connection::Exception& e) {
                throw ConnectionsTree::Operations::Exception(QObject::tr("Connection error: ") + QString(e.what()));
            }

            if (!scanningResp.isOkMessage())
                break;

            availableDatabeses.insert(dbIndex, 0);
            ++dbIndex;
        }

    }

    emit m_manager.openServerStats(m_connection);

    return callback(availableDatabeses);
}

void TreeOperations::getDatabaseKeys(uint dbIndex, QString filter,
                                     std::function<void (const RedisClient::Connection::RawKeysList &, const QString &)> callback)
{
    QString keyPattern = filter.isEmpty() ? static_cast<ServerConfig>(m_connection->getConfig()).keysPattern() : filter;

    try {
        if (m_connection->mode() == RedisClient::Connection::Mode::Cluster) {
            m_connection->getClusterKeys(callback, keyPattern);
        } else {
            m_connection->getDatabaseKeys(callback, keyPattern, dbIndex);
        }
    } catch (const RedisClient::Connection::Exception& error) {
        callback(RedisClient::Connection::RawKeysList(), QString(QObject::tr("Cannot load keys: %1")).arg(error.what()));
    }
}

void TreeOperations::disconnect()
{
    m_connection->disconnect();
}

QString TreeOperations::getNamespaceSeparator()
{
    return static_cast<ServerConfig>(m_connection->getConfig()).namespaceSeparator();
}

void TreeOperations::openKeyTab(ConnectionsTree::KeyItem& key, bool openInNewTab)
{
    emit m_manager.openValueTab(m_connection, key, openInNewTab);
}

void TreeOperations::openConsoleTab()
{    
    emit m_manager.openConsole(m_connection);
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
          callback(QString(QObject::tr("Cannot remove key: %1")).arg(error));
          return;
        }

        QRegExp filter(key.getFullPath(), Qt::CaseSensitive, QRegExp::Wildcard);
        emit m_manager.closeDbKeys(m_connection, key.getDbIndex(), filter);
        key.setRemoved();
    };

    try {
        m_connection->command({"DEL", key.getFullPath()}, this, cmdCallback, key.getDbIndex());
    } catch (const RedisClient::Connection::Exception& e) {
        throw ConnectionsTree::Operations::Exception(QObject::tr("Delete key error: ") + QString(e.what()));
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
          callback(QString(QObject::tr("Cannot remove key: %1")).arg(error));
          return;
        }
        callback(QString());
    };

    try {
        m_connection->command({"FLUSHDB"}, this, cmdCallback, dbIndex);
    } catch (const RedisClient::Connection::Exception& e) {
        throw ConnectionsTree::Operations::Exception(QObject::tr("FlushDB error: ") + QString(e.what()));
    }
}

QString TreeOperations::mode()
{
    if (m_connection->mode() == RedisClient::Connection::Mode::Cluster) {
        return QString("cluster");
    } else if (m_connection->mode() == RedisClient::Connection::Mode::Sentinel) {
        return QString("sentinel");
    } else {
        return QString("standalone");
    }
}
