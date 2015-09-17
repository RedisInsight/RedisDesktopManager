#pragma once
#include <qredisclient/connectionconfig.h>

class ConnectionConfig : public RedisClient::ConnectionConfig
{
public:
    static const char DEFAULT_NAMESPACE_SEPARATOR = ':';
    static const char DEFAULT_KEYS_GLOB_PATTERN = '*';

public:
    ConnectionConfig(const QString & host = "", const QString & auth = "",
                     const uint port = DEFAULT_REDIS_PORT, const QString & name = "");
    ConnectionConfig(const RedisClient::ConnectionConfig&);
    QString keysPattern() const;
    void setKeysPattern(QString keyGlobPattern);

    QString namespaceSeparator() const;
    void setNamespaceSeparator(QString);
};
