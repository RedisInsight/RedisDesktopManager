#include "tabmodel.h"


TabModel::TabModel(QSharedPointer<RedisClient::Connection> connection)
{
    // Clone connection
    RedisClient::ConnectionConfig config = connection->getConfig();
    m_connection = QSharedPointer<RedisClient::Connection>(new RedisClient::Connection(config));
}

QSharedPointer<RedisClient::Connection> TabModel::getConnection() const
{
    return m_connection;
}
