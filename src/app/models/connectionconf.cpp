#include "connectionconf.h"

ServerConfig::ServerConfig(const QString &host, const QString &auth, const uint port, const QString &name)
    : RedisClient::ConnectionConfig(host, auth, port, name)
{
    setParam<QString>("keys_pattern", QString(DEFAULT_KEYS_GLOB_PATTERN));
    setParam<QString>("namespace_separator", QString(DEFAULT_NAMESPACE_SEPARATOR));
}

ServerConfig::ServerConfig(const RedisClient::ConnectionConfig &other)
{
    m_parameters = other.getInternalParameters();
    m_owner = other.getOwner();
}

QString ServerConfig::keysPattern() const
{
    return (param<QString>("keys_pattern")).isEmpty() ?
                QString(DEFAULT_KEYS_GLOB_PATTERN) : param<QString>("keys_pattern");
}

void ServerConfig::setKeysPattern(QString keyGlobPattern)
{
    setParam<QString>("keys_pattern", keyGlobPattern);
}

QString ServerConfig::namespaceSeparator() const
{
    return param<QString>("namespace_separator");
}

void ServerConfig::setNamespaceSeparator(QString ns)
{
    return setParam<QString>("namespace_separator", ns);
}

bool ServerConfig::useSshTunnel() const
{
    return RedisClient::ConnectionConfig::useSshTunnel();
}
