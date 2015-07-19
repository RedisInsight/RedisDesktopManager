#include "connectionconfig.h"
#include <QFile>

RedisClient::ConnectionConfig::ConnectionConfig(const QString &host, const QString &name, const int port)    
{
    m_parameters.insert("name", name);
    m_parameters.insert("host", host);
    m_parameters.insert("port", port);
    m_parameters.insert("ssh_port", DEFAULT_SSH_PORT);
    m_parameters.insert("timeout_connect", DEFAULT_TIMEOUT_IN_MS);
    m_parameters.insert("timeout_execute", DEFAULT_TIMEOUT_IN_MS);
    m_parameters.insert("namespace_separator", QString(DEFAULT_NAMESPACE_SEPARATOR));
    m_parameters.insert("keys_pattern", QString("*"));
}

RedisClient::ConnectionConfig &RedisClient::ConnectionConfig::operator =(const ConnectionConfig &other)
{
    if (this != &other) {
        m_parameters = other.m_parameters;
        m_owner = other.m_owner;
    }

    return *this;
}

RedisClient::ConnectionConfig::ConnectionConfig(const QVariantHash &options)
    : m_parameters(options)
{
}

QString RedisClient::ConnectionConfig::name() const
{
    return param<QString>("name");
}

QString RedisClient::ConnectionConfig::host() const
{
    return param<QString>("host");
}

QString RedisClient::ConnectionConfig::auth() const
{
    return param<QString>("auth");
}

int RedisClient::ConnectionConfig::port() const
{
    return param<int>("port");
}

int RedisClient::ConnectionConfig::executeTimeout() const
{
    return param<int>("timeout_execute");
}

int RedisClient::ConnectionConfig::connectionTimeout() const
{
    return param<int>("timeout_connect");
}

QList<QSslCertificate> RedisClient::ConnectionConfig::sslCaCertificates() const
{
    QString path = param<QString>("ssl_ca_cert_path");
    if (!path.isEmpty() && QFile::exists(path))
        return QSslCertificate::fromPath(path);

    return QList<QSslCertificate>();
}

QString RedisClient::ConnectionConfig::sslPrivateKeyPath() const
{
    return getValidPathFromParameter("ssl_private_key_path");
}

QString RedisClient::ConnectionConfig::sslLocalCertPath() const
{
    return getValidPathFromParameter("ssl_local_cert_path");
}

bool RedisClient::ConnectionConfig::isSshPasswordUsed()
{
    return !param<QString>("ssh_password").isEmpty();
}

QVariantHash RedisClient::ConnectionConfig::getInternalParameters()
{
    return m_parameters;
}

void RedisClient::ConnectionConfig::setSshTunnelSettings(QString host, QString user, QString pass, int port, QString privateKey)
{
    m_parameters.insert("ssh_host", host);
    m_parameters.insert("ssh_user", user);
    m_parameters.insert("ssh_password", pass);
    m_parameters.insert("ssh_port", port);
    m_parameters.insert("ssh_private_key_path", privateKey);
}

bool RedisClient::ConnectionConfig::isNull() const
{
    return param<QString>("host").isEmpty()
            || param<int>("port") <= 0
            || param<QString>("name").isEmpty();
}

bool RedisClient::ConnectionConfig::useSshTunnel() const
{
    return !param<QString>("ssh_host").isEmpty()
            && param<int>("ssh_port") > 0
            && !param<QString>("ssh_user").isEmpty()
            && (!param<QString>("ssh_password").isEmpty()
                || !param<QString>("ssh_private_key_path").isEmpty());
}

bool RedisClient::ConnectionConfig::useAuth() const
{
    return !param<QString>("auth").isEmpty();
}

bool RedisClient::ConnectionConfig::useSsl() const
{
    return !param<QString>("ssl_ca_cert_path").isEmpty();
}

bool RedisClient::ConnectionConfig::isValid() const
{
    return isNull() == false
            && param<int>("timeout_connect") > 1000
            && param<int>("timeout_execute") > 1000;
}

void RedisClient::ConnectionConfig::setOwner(QWeakPointer<RedisClient::Connection> owner)
{
    m_owner = owner;
}

QWeakPointer<RedisClient::Connection> RedisClient::ConnectionConfig::getOwner() const
{
    return m_owner;
}

QString RedisClient::ConnectionConfig::getSshPrivateKey()
{
    return getValidPathFromParameter("ssh_private_key_path");
}

QString RedisClient::ConnectionConfig::keysPattern() const
{
    return param<QString>("keys_pattern");
}

RedisClient::ConnectionConfig RedisClient::ConnectionConfig::fromJsonObject(const QJsonObject &config)
{
    QVariantHash options = config.toVariantHash();
    ConnectionConfig c(options);
    return c;
}

QJsonObject RedisClient::ConnectionConfig::toJsonObject()
{
    return QJsonObject::fromVariantHash(m_parameters);
}

QString RedisClient::ConnectionConfig::getValidPathFromParameter(const QString &name) const
{
    QString path = param<QString>(name);
    if (path.isEmpty() || !QFile::exists(path))
        return QString();

    return path;
}
