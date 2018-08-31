#include "tabmodel.h"


TabModel::TabModel(QSharedPointer<RedisClient::Connection> connection, int dbIndex)
{
    // Clone connection
    RedisClient::ConnectionConfig config = connection->getConfig();
    m_connection = QSharedPointer<RedisClient::Connection>(new RedisClient::Connection(config));

    if (dbIndex > 0) {
        m_connection->commandSync("PING", dbIndex);
    }
}

QSharedPointer<RedisClient::Connection> TabModel::getConnection() const
{
    return m_connection;
}
