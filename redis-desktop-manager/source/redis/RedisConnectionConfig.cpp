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

const QString& RedisConnectionConfig::getName()
{
    return name;
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
    int port = DEFAULT_REDIS_PORT;

    if (attr.contains("port")) {
        port = attr.namedItem("port").nodeValue().toInt();
    }

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

    QDomAttr nameAttr = dom.createAttribute("name");
    nameAttr.setValue(name);
    xml.setAttributeNode(nameAttr);

    QDomAttr hostAttr = dom.createAttribute("host");
    hostAttr.setValue(host);
    xml.setAttributeNode(hostAttr);

    QDomAttr portAttr = dom.createAttribute("port");
    portAttr.setValue(QString("%1").arg(port));
    xml.setAttributeNode(portAttr);

    if (useAuth()) {
        QDomAttr authAttr = dom.createAttribute("auth");
        authAttr.setValue(auth);
        xml.setAttributeNode(authAttr);
    }

    if (namespaceSeparator != QString(DEFAULT_NAMESPACE_SEPARATOR)) {
        QDomAttr nsAttr = dom.createAttribute("namespaceSeparator");
        nsAttr.setValue(namespaceSeparator);
        xml.setAttributeNode(nsAttr);
    }

    if (useSshTunnel()) {
        QDomAttr sshHostAttr = dom.createAttribute("sshHost");
        sshHostAttr.setValue(sshHost);
        xml.setAttributeNode(sshHostAttr);

        QDomAttr sshUserAttr = dom.createAttribute("sshUser");
        sshUserAttr.setValue(sshUser);
        xml.setAttributeNode(sshUserAttr);

        QDomAttr sshPassAttr = dom.createAttribute("sshPassword");
        sshPassAttr.setValue(sshPassword);
        xml.setAttributeNode(sshPassAttr);

        QDomAttr sshPortAttr = dom.createAttribute("sshPort");
        sshPortAttr.setValue(QString("%1").arg(sshPort));
        xml.setAttributeNode(sshPortAttr);
    }

    return xml;
}
