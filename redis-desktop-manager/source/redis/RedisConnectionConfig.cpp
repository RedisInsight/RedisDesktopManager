#include "RedisConnectionConfig.h"

void RedisConnectionConfig::setSshTunnelSettings(QString host, QString user, QString pass, int port) 
{
    sshHost = host;
    sshUser = user;
    sshPassword = pass;
    sshPort = port;
}; 


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
    getValueFromXml(attr, "namespaceSeparator", connectionConfig.namespaceSeparator);    
    getValueFromXml(attr, "sshHost", connectionConfig.sshHost);    
    getValueFromXml(attr, "sshUser", connectionConfig.sshUser);    
    getValueFromXml(attr, "sshPassword", connectionConfig.sshPassword);    
    getValueFromXml(attr, "sshPort", connectionConfig.sshPort);

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

    value = val.toInt();

    return result;
}

QDomElement RedisConnectionConfig::toXml(QDomDocument dom)
{
    QDomElement xml = dom.createElement("connection");

    saveXmlAttribute(dom, xml, "name", name);    
    saveXmlAttribute(dom, xml, "host", host);   
    saveXmlAttribute(dom, xml, "port", QString("%1").arg(port));   

    if (useAuth()) {
        saveXmlAttribute(dom, xml, "auth", auth); 
    }

    if (namespaceSeparator != QString(DEFAULT_NAMESPACE_SEPARATOR)) {
        saveXmlAttribute(dom, xml, "namespaceSeparator", namespaceSeparator); 
    }

    if (useSshTunnel()) {
        saveXmlAttribute(dom, xml, "sshHost", sshHost); 
        saveXmlAttribute(dom, xml, "sshUser", sshUser); 
        saveXmlAttribute(dom, xml, "sshPassword", sshPassword); 
        saveXmlAttribute(dom, xml, "sshPort", QString("%1").arg(sshPort)); 
    }

    return xml;
}

void RedisConnectionConfig::saveXmlAttribute(QDomDocument & document, QDomElement & root, const QString& name, const QString& value)
{
    QDomAttr attr = document.createAttribute(name);
    attr.setValue(value);
    root.setAttributeNode(attr);
}
