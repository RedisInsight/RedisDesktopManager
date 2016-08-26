#pragma once
#include <QObject>
#include <qredisclient/connectionconfig.h>

class ServerConfig : public RedisClient::ConnectionConfig
{
    Q_GADGET

    /* Basic settings */
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString host READ host WRITE setHost)
    Q_PROPERTY(uint port READ port WRITE setPort)
    Q_PROPERTY(QString auth READ auth WRITE setAuth)

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

    /* DEFAULTS */
    Q_PROPERTY(QString defautNamespaceSeparator MEMBER DEFAULT_NAMESPACE_SEPARATOR CONSTANT)
    Q_PROPERTY(QString defautKeysGlobPattern MEMBER DEFAULT_KEYS_GLOB_PATTERN CONSTANT)
    Q_PROPERTY(uint defautPort MEMBER DEFAULT_REDIS_PORT CONSTANT)
    Q_PROPERTY(uint defautSshPort MEMBER DEFAULT_SSH_PORT CONSTANT)
    Q_PROPERTY(uint defautTimeoutInMs MEMBER DEFAULT_TIMEOUT_IN_MS CONSTANT)

public:
    static const char DEFAULT_NAMESPACE_SEPARATOR = ':';
    static const char DEFAULT_KEYS_GLOB_PATTERN = '*';

public:
    ServerConfig(const QString & host = "127.0.0.1", const QString & auth = "",
                     const uint port = DEFAULT_REDIS_PORT, const QString & name = "");

    ServerConfig(const RedisClient::ConnectionConfig&);

    QString keysPattern() const;
    void setKeysPattern(QString keyGlobPattern);

    QString namespaceSeparator() const;
    void setNamespaceSeparator(QString);

    Q_INVOKABLE bool useSshTunnel() const;
};

Q_DECLARE_METATYPE(ServerConfig)
