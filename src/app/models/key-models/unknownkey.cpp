#include "unknownkey.h"
#include <qredisclient/connection.h>

UnknownKeyModel::UnknownKeyModel(
    QSharedPointer<RedisClient::Connection> connection, QByteArray fullPath,
    int dbIndex, long long ttl, QString type)
    : KeyModel(connection, fullPath, dbIndex, ttl), m_type(type) {}
