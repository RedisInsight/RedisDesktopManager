#include "connectionconf.h"

ServerConfig::ServerConfig(const QString &host, const QString &auth, const uint port, const QString &name)
    : RedisClient::ConnectionConfig(host, auth, port, name)
{
}

ServerConfig::ServerConfig(const RedisClient::ConnectionConfig &other)
{
    m_parameters = other.getInternalParameters();
    m_owner = other.getOwner();
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

bool ServerConfig::luaKeysLoading() const
{
    return param<bool>("lua_keys_loading", DEFAULT_LUA_KEYS_LOADING);
}

void ServerConfig::setLuaKeysLoading(bool value)
{
    return setParam<bool>("lua_keys_loading", value);
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
