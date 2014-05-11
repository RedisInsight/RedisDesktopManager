#include "connectionconfig.h"
#include <QFile>

bool RedisClient::ConnectionConfig::isSshPasswordUsed()
{
    return !sshPassword.isNull() && !sshPassword.isEmpty();
}

RedisClient::ConnectionConfig::ConnectionConfig(const QString &host, const QString &name, const int port)
    : name(name), host(host), port(port), sshPort(DEFAULT_SSH_PORT), connectionTimeout(DEFAULT_TIMEOUT_IN_MS),
      executeTimeout(DEFAULT_TIMEOUT_IN_MS), namespaceSeparator(DEFAULT_NAMESPACE_SEPARATOR)
{}

RedisClient::ConnectionConfig &RedisClient::ConnectionConfig::operator =(RedisClient::ConnectionConfig &other)
{
    if (this != &other) {
        name = other.name;
        host = other.host;
        auth = other.auth;
        port = other.port;
        namespaceSeparator = other.namespaceSeparator;
        connectionTimeout = other.connectionTimeout;
        executeTimeout = other.executeTimeout;

        setSshTunnelSettings(
                    other.sshHost, other.sshUser, other.sshPassword,
                    other.sshPort, other.sshPrivateKeyPath
                    );
    }

    return *this;
}

void RedisClient::ConnectionConfig::setSshTunnelSettings(QString host, QString user, QString pass, int port, QString privateKey)
{
    sshHost = host;
    sshUser = user;
    sshPassword = pass;
    sshPort = port;    
    sshPrivateKeyPath = privateKey;
}

bool RedisClient::ConnectionConfig::isNull()
{
    return host.isEmpty() || port <= 0 || name.isEmpty();
}

bool RedisClient::ConnectionConfig::useSshTunnel() const
{
    return !sshHost.isEmpty()
            && sshPort > 0
            && !sshUser.isEmpty()
            && (!sshPassword.isEmpty() || !sshPrivateKeyPath.isEmpty());
}

bool RedisClient::ConnectionConfig::useAuth() const
{
    return !(auth.isEmpty());
}

QString RedisClient::ConnectionConfig::getSshPrivateKey()
{
    if (sshPrivateKeyPath.isEmpty()
            || !QFile::exists(sshPrivateKeyPath))
        return QString();

    return sshPrivateKeyPath;
}

RedisClient::ConnectionConfig RedisClient::ConnectionConfig::createFromXml(QDomNode & connectionNode)
{
    ConnectionConfig connectionConfig;

    if (!connectionNode.hasAttributes()) {
        return connectionConfig;
    }

    QDomNamedNodeMap attr = connectionNode.attributes();

    getValueFromXml(attr, "name", connectionConfig.name);
    getValueFromXml(attr, "host", connectionConfig.host);
    getValueFromXml(attr, "port", connectionConfig.port);    
    getValueFromXml(attr, "auth", connectionConfig.auth);        

    getValueFromXml(attr, "sshHost", connectionConfig.sshHost);    
    getValueFromXml(attr, "sshUser", connectionConfig.sshUser);    
    getValueFromXml(attr, "sshPassword", connectionConfig.sshPassword);
    getValueFromXml(attr, "sshPort", connectionConfig.sshPort);    
    getValueFromXml(attr, "sshPrivateKey", connectionConfig.sshPrivateKeyPath);

    getValueFromXml(attr, "namespaceSeparator", connectionConfig.namespaceSeparator);   
    getValueFromXml(attr, "connectionTimeout", connectionConfig.connectionTimeout);   
    getValueFromXml(attr, "executeTimeout", connectionConfig.executeTimeout);   

    return connectionConfig;
}

bool RedisClient::ConnectionConfig::getValueFromXml(const QDomNamedNodeMap & attr, const QString& name, QString & value)
{
    if (!attr.contains(name))
        return false;

    value = attr.namedItem(name).nodeValue();

    return true;
}

bool RedisClient::ConnectionConfig::getValueFromXml(const QDomNamedNodeMap & attr, const QString& name, int & value)
{
    QString val;

    bool result = getValueFromXml(attr, name, val);

    if (result) {
        value = val.toInt();
    }    

    return result;
}

QDomElement RedisClient::ConnectionConfig::toXml(QDomDocument dom)
{
    QDomElement xml = dom.createElement("connection");

    saveXmlAttribute(dom, xml, "name", name);    
    saveXmlAttribute(dom, xml, "host", host);   
    saveXmlAttribute(dom, xml, "port", QString::number(port));   

    if (useAuth()) {
        saveXmlAttribute(dom, xml, "auth", auth); 
    }

    if (namespaceSeparator != QString(DEFAULT_NAMESPACE_SEPARATOR)) {
        saveXmlAttribute(dom, xml, "namespaceSeparator", namespaceSeparator); 
    }

    saveXmlAttribute(dom, xml, "connectionTimeout", QString::number(connectionTimeout)); 
    saveXmlAttribute(dom, xml, "executeTimeout", QString::number(executeTimeout)); 

    if (useSshTunnel()) {
        saveXmlAttribute(dom, xml, "sshHost", sshHost); 
        saveXmlAttribute(dom, xml, "sshUser", sshUser); 
        saveXmlAttribute(dom, xml, "sshPassword", sshPassword); 
        saveXmlAttribute(dom, xml, "sshPort", QString::number(sshPort)); 
        saveXmlAttribute(dom, xml, "sshPrivateKey", sshPrivateKeyPath);        
    }

    return xml;
}

void RedisClient::ConnectionConfig::saveXmlAttribute(QDomDocument & document, QDomElement & root, const QString& name, const QString& value)
{
    QDomAttr attr = document.createAttribute(name);
    attr.setValue(value);
    root.setAttributeNode(attr);
}
