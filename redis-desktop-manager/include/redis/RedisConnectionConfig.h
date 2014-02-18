#pragma once

#include <QtCore>
#include <QtXml>

#define DEFAULT_REDIS_PORT 6379
#define DEFAULT_SSH_PORT 22
#define DEFAULT_TIMEOUT_IN_MS 60000

class RedisConnectionConfig
{
public:
    // redis connection parameters
    QString name;
    QString host;
    QString auth;
    int port;

    // ssh tunnel connection parameters
    QString sshHost;
    int sshPort;
    QString sshUser;
    QString sshPassword;

    //timeouts 
    int connectionTimeout;
    int executeTimeout;

    // other settings
    QString namespaceSeparator;    
    static const char DEFAULT_NAMESPACE_SEPARATOR = ':';

    RedisConnectionConfig(const QString & host, const QString & name = "", const int port = DEFAULT_REDIS_PORT) 
        : name(name), host(host), port(port), sshPort(DEFAULT_SSH_PORT), namespaceSeparator(DEFAULT_NAMESPACE_SEPARATOR),
          connectionTimeout(DEFAULT_TIMEOUT_IN_MS), executeTimeout(DEFAULT_TIMEOUT_IN_MS)
    {};

    RedisConnectionConfig & operator = (RedisConnectionConfig & other) 
    {
        if (this != &other) {
            name = other.name;
            host = other.host;
            auth = other.auth;
            port = other.port;
            namespaceSeparator = other.namespaceSeparator;

            setSshTunnelSettings(other.sshHost, other.sshUser, other.sshPassword, other.sshPort);
        }

        return *this;
    }

    void setSshTunnelSettings(QString host, QString user, QString pass, int port = DEFAULT_SSH_PORT);
    
    bool isNull();
    bool useSshTunnel() const;
    bool useAuth() const;
    
    QDomElement toXml(QDomDocument dom);    

    static RedisConnectionConfig createFromXml(QDomNode & connectionNode);
};

