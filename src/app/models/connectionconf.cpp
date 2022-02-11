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

QVariantMap ServerConfig::filterHistory()
{
    return param<QVariantMap>("filter_history");
}

void ServerConfig::setFilterHistory(QVariantMap filterHistory)
{
    setParam<QVariantMap>("filter_history", filterHistory);
}

bool ServerConfig::askForSshPassword() const
{
    return param<bool>("ask_ssh_password", false);
}

void ServerConfig::setAskForSshPassword(bool v)
{
    setParam<bool>("ask_ssh_password", v);
}

QString ServerConfig::defaultFormatter() const
{
    return param<QString>("default_formatter", QString("auto"));
}

void ServerConfig::setDefaultFormatter(const QString &v)
{
    setParam<QString>("default_formatter", v);
}

QString ServerConfig::iconColor() const
{
    return param<QString>("icon_color", QString(""));
}

void ServerConfig::setIconColor(const QString &v)
{
    setParam<QString>("icon_color", v);
}
