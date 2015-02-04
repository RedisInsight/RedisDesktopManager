#pragma once

#include <QtCore>
#include <QtXml>

#define DEFAULT_REDIS_PORT 6379
#define DEFAULT_SSH_PORT 22
#define DEFAULT_TIMEOUT_IN_MS 60000

namespace RedisClient {

class Connection;

class ConnectionConfig
{
public:
    ConnectionConfig(const QString & host = "", const QString & name = "", const int port = DEFAULT_REDIS_PORT);
    ConnectionConfig & operator = (const ConnectionConfig & other);

    bool isNull() const;
    bool useSshTunnel() const;
    bool useAuth() const;
    bool isValid() const;
    void setOwner(QWeakPointer<Connection>);
    QWeakPointer<Connection> getOwner() const;

    void setSshTunnelSettings(QString host, QString user, QString pass, int port = DEFAULT_SSH_PORT,
                              QString sshPrivatekey = "");
    QString getSshPrivateKey();

    QDomElement toXml(QDomDocument dom);
    QStringList allowedNamespaces();

    static ConnectionConfig createFromXml(QDomNode & connectionNode);
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
    QString sshPrivateKeyPath;    

    bool isSshPasswordUsed();

    //timeouts 
    int connectionTimeout;
    int executeTimeout;    

    // other settings
    QString namespaceSeparator;    
    static const char DEFAULT_NAMESPACE_SEPARATOR = ':';
    QString defaultValueFormat;

protected:        
    void saveXmlAttribute(QDomDocument & document, QDomElement & root, const QString& name, const QString& value);

    static bool getValueFromXml(const QDomNamedNodeMap & attr, const QString& name, QString & value);
    static bool getValueFromXml(const QDomNamedNodeMap & attr, const QString& name, int & value);

private:
    QWeakPointer<Connection> m_owner;
};

}
