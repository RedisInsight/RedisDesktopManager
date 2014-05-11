#pragma once

#include <QtCore>
#include <QtXml>

#define DEFAULT_REDIS_PORT 6379
#define DEFAULT_SSH_PORT 22
#define DEFAULT_TIMEOUT_IN_MS 60000

namespace RedisClient {

class ConnectionConfig
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
    QString sshPrivateKeyPath;    

    bool isSshPasswordUsed();

    //timeouts 
    int connectionTimeout;
    int executeTimeout;    

    // other settings
    QString namespaceSeparator;    
    static const char DEFAULT_NAMESPACE_SEPARATOR = ':';

    ConnectionConfig(const QString & host = "", const QString & name = "", const int port = DEFAULT_REDIS_PORT);

    ConnectionConfig & operator = (ConnectionConfig & other);

    void setSshTunnelSettings(QString host, QString user, QString pass, int port = DEFAULT_SSH_PORT, 
                                QString sshPrivatekey = "");
    
    bool isNull();
    bool useSshTunnel() const;
    bool useAuth() const;
    
    QString getSshPrivateKey();    

    QDomElement toXml(QDomDocument dom);  
       
    static ConnectionConfig createFromXml(QDomNode & connectionNode);

protected:        
    void saveXmlAttribute(QDomDocument & document, QDomElement & root, const QString& name, const QString& value);

    static bool getValueFromXml(const QDomNamedNodeMap & attr, const QString& name, QString & value);
    static bool getValueFromXml(const QDomNamedNodeMap & attr, const QString& name, int & value);
};

}
