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
    if (!connectionNode.hasAttributes()) {
        return RedisConnectionConfig("");
    }

    QDomNamedNodeMap attr = connectionNode.attributes();

    if (!attr.contains("name") || !attr.contains("host")) {
        return RedisConnectionConfig("");
    }

    QString name = attr.namedItem("name").nodeValue();
    QString host = attr.namedItem("host").nodeValue();

    int port = attr.contains("port") ? 
        attr.namedItem("port").nodeValue().toInt() : DEFAULT_REDIS_PORT;

    RedisConnectionConfig connectionConfig(host, name, port);

    if (attr.contains("auth")) {
        connectionConfig.auth = attr.namedItem("auth").nodeValue();
    }

    if (attr.contains("namespaceSeparator")) {
        connectionConfig.namespaceSeparator = attr.namedItem("namespaceSeparator").nodeValue();
    }

    if (attr.contains("sshHost") 
        && attr.contains("sshUser")
        && attr.contains("sshPassword")) {
            QString sshHost = attr.namedItem("sshHost").nodeValue();
            QString sshUser = attr.namedItem("sshUser").nodeValue();
            QString sshPassword = attr.namedItem("sshPassword").nodeValue();

            int sshPort = attr.contains("sshPort") ? 
                attr.namedItem("sshPort").nodeValue().toInt() : DEFAULT_SSH_PORT;

            connectionConfig.setSshTunnelSettings(sshHost, sshUser, sshPassword, sshPort);
    }

    return connectionConfig;
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
