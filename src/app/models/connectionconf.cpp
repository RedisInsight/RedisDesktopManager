#include "connectionconf.h"

ServerConfig::ServerConfig(const QString &host, const QString &auth, const uint port, const QString &name)
    : RedisClient::ConnectionConfig(host, auth, port, name),
      m_owner(QSharedPointer<TreeOperations>())
{
}

ServerConfig::ServerConfig(const QVariantHash &options)
    : RedisClient::ConnectionConfig(options),
      m_owner(QSharedPointer<TreeOperations>())
{

}

ServerConfig::ServerConfig(const ServerConfig &other)
{
    m_parameters = other.m_parameters;
    m_owner = other.m_owner;
}

QString ServerConfig::keysPattern() const
{
    return param<QString>("keys_pattern", QString(DEFAULT_KEYS_GLOB_PATTERN));
}

void ServerConfig::setKeysPattern(QString keyGlobPattern)
{
    setParam<QString>("keys_pattern", keyGlobPattern);
}

QString ServerConfig::namespaceSeparator() const
{
    return param<QString>("namespace_separator", QString(DEFAULT_NAMESPACE_SEPARATOR));
}

void ServerConfig::setNamespaceSeparator(QString ns)
{
    return setParam<QString>("namespace_separator", ns);
}

uint ServerConfig::databaseScanLimit() const
{
    return param<uint>("db_scan_limit", DEFAULT_DB_SCAN_LIMIT);
}

void ServerConfig::setDatabaseScanLimit(uint limit)
{
    setParam<uint>("db_scan_limit", limit);
}

bool ServerConfig::useSshTunnel() const
{
    return RedisClient::ConnectionConfig::useSshTunnel();
}

QWeakPointer<TreeOperations> ServerConfig::owner() const
{
    return m_owner;
}

void ServerConfig::setOwner(QWeakPointer<TreeOperations> o)
{
    m_owner = o;
}
