#include "treeoperations.h"
#include <qredisclient/redisclient.h>
#include "app/models/connectionconf.h"

#include <algorithm>
#include <QSet>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>

TreeOperations::TreeOperations(QSharedPointer<RedisClient::Connection> connection)
    : m_connection(connection)
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

void TreeOperations::getDatabaseKeys(uint dbIndex, QString filter, std::function<void (const RedisClient::Connection::RawKeysList &, const QString &)> callback)
{
    QString keyPattern = filter.isEmpty() ? static_cast<ServerConfig>(m_connection->getConfig()).keysPattern() : filter;

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
    return static_cast<ServerConfig>(m_connection->getConfig()).namespaceSeparator();
}

void TreeOperations::openKeyTab(ConnectionsTree::KeyItem& key, bool openInNewTab)
{
    emit openValueTab(m_connection, key, openInNewTab);
}

void TreeOperations::openConsoleTab()
{    
    emit openConsole(m_connection);
}

void TreeOperations::openNewKeyDialog(int dbIndex, std::function<void()> callback,
                                      QString keyPrefix)
{
    emit newKeyDialog(m_connection, callback, dbIndex, keyPrefix);
}

void TreeOperations::notifyDbWasUnloaded(int dbIndex)
{
    emit closeDbKeys(m_connection, dbIndex);
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
        emit closeDbKeys(m_connection, key.getDbIndex(), filter);
        key.setRemoved();
    };

    try {
        m_connection->command({"DEL", key.getFullPath()}, this, cmdCallback, key.getDbIndex());
    } catch (const RedisClient::Connection::Exception& e) {
        throw ConnectionsTree::Operations::Exception("Delete key error: " + QString(e.what()));
    }
}
