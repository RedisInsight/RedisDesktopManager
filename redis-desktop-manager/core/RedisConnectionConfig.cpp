#include "RedisConnectionConfig.h"

RedisConnectionConfig::SshAuthType RedisConnectionConfig::getSshAuthType()
{
    return sshAuth;
}

bool RedisConnectionConfig::isSshPasswordUsed()
{
    return !sshPassword.isNull() && !sshPassword.isEmpty();
}

RedisConnectionConfig::RedisConnectionConfig(const QString &host, const QString &name, const int port)
    : name(name), host(host), port(port), sshPort(DEFAULT_SSH_PORT), sshAuth(RedisConnectionConfig::SshAuthType::None),
     connectionTimeout(DEFAULT_TIMEOUT_IN_MS), executeTimeout(DEFAULT_TIMEOUT_IN_MS), namespaceSeparator(DEFAULT_NAMESPACE_SEPARATOR)
{}

RedisConnectionConfig &RedisConnectionConfig::operator =(RedisConnectionConfig &other)
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
                    other.sshHost, other.sshUser, other.sshPassword, other.sshPort,
                    other.sshPublicKey, other.sshPublicKey
                    );
    }

    return *this;
}

void RedisConnectionConfig::setSshTunnelSettings(QString host, QString user, QString pass, int port,
                                                 QString publicKey, QString privateKey)
{
    sshHost = host;
    sshUser = user;
    sshPassword = pass;
    sshPort = port;
    sshPublicKey = publicKey;
    sshPrivateKey = privateKey;
}

bool RedisConnectionConfig::isNull() 
{
    return host.isEmpty() || port <= 0 || name.isEmpty();
}

bool RedisConnectionConfig::useSshTunnel() const
{
    return !sshHost.isEmpty()
            && !sshUser.isEmpty()
            && !sshPassword.isEmpty()
            && sshPort > 0;
}

bool RedisConnectionConfig::useAuth() const
{
    return !(auth.isEmpty());
}

RedisConnectionConfig RedisConnectionConfig::createFromXml(QDomNode & connectionNode) 
{
    RedisConnectionConfig connectionConfig;

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

    getValueFromXml(attr, "sshPublicKey", connectionConfig.sshPublicKey);
    getValueFromXml(attr, "sshPrivateKey", connectionConfig.sshPrivateKey);

    getValueFromXml(attr, "namespaceSeparator", connectionConfig.namespaceSeparator);   
    getValueFromXml(attr, "connectionTimeout", connectionConfig.connectionTimeout);   
    getValueFromXml(attr, "executeTimeout", connectionConfig.executeTimeout);   

    return connectionConfig;
}

bool RedisConnectionConfig::getValueFromXml(const QDomNamedNodeMap & attr, const QString& name, QString & value)
{
    if (!attr.contains(name))
        return false;

    value = attr.namedItem(name).nodeValue();

    return true;
}

bool RedisConnectionConfig::getValueFromXml(const QDomNamedNodeMap & attr, const QString& name, int & value)
{
    QString val;

    bool result = getValueFromXml(attr, name, val);

    if (result) {
        value = val.toInt();
    }    

    return result;
}

QDomElement RedisConnectionConfig::toXml(QDomDocument dom)
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

        saveXmlAttribute(dom, xml, "sshPrivateKey", sshPrivateKey); 
        saveXmlAttribute(dom, xml, "sshPublicKey", sshPublicKey);
    }

    return xml;
}

void RedisConnectionConfig::saveXmlAttribute(QDomDocument & document, QDomElement & root, const QString& name, const QString& value)
{
    QDomAttr attr = document.createAttribute(name);
    attr.setValue(value);
    root.setAttributeNode(attr);
}
