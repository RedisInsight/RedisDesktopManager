#pragma once
#include <QObject>
#include <qredisclient/connectionconfig.h>


class TreeOperations;

class ServerConfig : public RedisClient::ConnectionConfig
{
    Q_GADGET

    /* Basic settings */
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString host READ host WRITE setHost)
    Q_PROPERTY(uint port READ port WRITE setPort)
    Q_PROPERTY(QString auth READ auth WRITE setAuth)
    Q_PROPERTY(QString username READ username WRITE setUsername)

    /* SSL settings */
    Q_PROPERTY(bool sslEnabled READ useSsl WRITE setSsl)
    Q_PROPERTY(QString sslLocalCertPath READ sslLocalCertPath WRITE setSslLocalCertPath)
    Q_PROPERTY(QString sslPrivateKeyPath READ sslPrivateKeyPath WRITE setSslPrivateKeyPath)
    Q_PROPERTY(QString sslCaCertPath READ sslCaCertPath WRITE setSslCaCertPath)

    /* SSH Settings */
    Q_PROPERTY(QString sshPassword READ sshPassword WRITE setSshPassword)
    Q_PROPERTY(QString sshUser READ sshUser WRITE setSshUser)
    Q_PROPERTY(QString sshHost READ sshHost WRITE setSshHost)
    Q_PROPERTY(uint sshPort READ sshPort WRITE setSshPort)
    Q_PROPERTY(QString sshPrivateKey READ getSshPrivateKeyPath WRITE setSshPrivateKeyPath)

    /* Advanced settings */
    Q_PROPERTY(QString keysPattern READ keysPattern WRITE setKeysPattern)
    Q_PROPERTY(QString namespaceSeparator READ namespaceSeparator WRITE setNamespaceSeparator)
    Q_PROPERTY(uint executeTimeout READ executeTimeout WRITE setExecutionTimeout)
    Q_PROPERTY(uint connectionTimeout READ connectionTimeout WRITE setConnectionTimeout)    
    Q_PROPERTY(bool overrideClusterHost READ overrideClusterHost WRITE setClusterHostOverride)
    Q_PROPERTY(bool ignoreSSLErrors READ ignoreAllSslErrors WRITE setIgnoreAllSslErrors)
    Q_PROPERTY(uint databaseScanLimit READ databaseScanLimit WRITE setDatabaseScanLimit)



public:
    static const char DEFAULT_NAMESPACE_SEPARATOR = ':';
    static const char DEFAULT_KEYS_GLOB_PATTERN = '*';
    static const bool DEFAULT_LUA_KEYS_LOADING = false;
    static const uint DEFAULT_DB_SCAN_LIMIT = 20;

public:
    ServerConfig(const QString & host = "127.0.0.1", const QString & auth = "",
                     const uint port = DEFAULT_REDIS_PORT, const QString & name = "");

    ServerConfig(const QVariantHash& options);

    ServerConfig(const ServerConfig& options);

    QString keysPattern() const;
    void setKeysPattern(QString keyGlobPattern);

    QString namespaceSeparator() const;
    void setNamespaceSeparator(QString);

    bool luaKeysLoading() const;
    void setLuaKeysLoading(bool);

    uint databaseScanLimit() const;
    void setDatabaseScanLimit(uint limit);

    Q_INVOKABLE bool useSshTunnel() const;

    QWeakPointer<TreeOperations> owner() const;
    void setOwner(QWeakPointer<TreeOperations> o);

private:
    QWeakPointer<TreeOperations> m_owner;
};

Q_DECLARE_METATYPE(ServerConfig)
