#include "RedisConnectionConfig.h"
#include <QFile>

void RedisConnectionConfig::setSshTunnelSettings(QString host, QString user, QString pass, int port, 
                                                 QString publicKey, QString privateKey) 
{
    sshHost = host;
    sshUser = user;
    sshPassword = pass;
    sshPort = port;
    sshPublicKeyPath = publicKey;
    sshPrivateKeyPath = privateKey;
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

QString RedisConnectionConfig::getSshPrivateKey()
{
    if (sshPrivateKeyPath.isEmpty() || sshPublicKeyPath.isNull()
            || !QFile.exists(sshPrivateKeyPath))
        return QString;

    QFile keyFile(sshPrivateKeyPath);
    if (!keyFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString;

    QTextStream in(&keyFile);
    QString keyString = in.readAll();

    return keyString;
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

    getValueFromXml(attr, "sshPublicKey", connectionConfig.sshPublicKeyPath);
    getValueFromXml(attr, "sshPrivateKey", connectionConfig.sshPrivateKeyPath);

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

        saveXmlAttribute(dom, xml, "sshPrivateKey", sshPrivateKeyPath);
        saveXmlAttribute(dom, xml, "sshPublicKey", sshPublicKeyPath);
    }

    return xml;
}

void RedisConnectionConfig::saveXmlAttribute(QDomDocument & document, QDomElement & root, const QString& name, const QString& value)
{
    QDomAttr attr = document.createAttribute(name);
    attr.setValue(value);
    root.setAttributeNode(attr);
}
