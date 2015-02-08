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
}

RedisClient::ConnectionConfig &RedisClient::ConnectionConfig::operator =(const ConnectionConfig &other)
{
    if (this != &other) {
        m_parameters = other.m_parameters;
        m_owner = other.m_owner;
    }

    return *this;
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

bool RedisClient::ConnectionConfig::isSshPasswordUsed()
{
    return !param<QString>("ssh_password").isEmpty();
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
    QString path = param<QString>("ssh_private_key_path");
    if (path.isEmpty() || !QFile::exists(path))
        return QString();

    return path;
}

RedisClient::ConnectionConfig RedisClient::ConnectionConfig::fromXml(QDomNode & connectionNode)
{
    ConnectionConfig c;

    if (!connectionNode.hasAttributes()) {
        return c;
    }

    QDomNamedNodeMap attr = connectionNode.attributes();

    QHash<QString, QString> valueMapping({
        {"name", ""}, {"host", ""}, {"port", ""}, {"auth", ""},
        {"sshHost", "ssh_host"}, {"sshUser", "ssh_user"}, {"sshPassword", "ssh_password"},
        {"sshPort", "ssh_port"}, {"sshPrivateKey", "ssh_private_key_path"},
        {"namespaceSeparator", "namespace_separator"},
        {"connectionTimeout", "timeout_connect"}, {"executeTimeout", "timeout_execute"}
    });

    QHashIterator<QString, QString> i(valueMapping);

    while (i.hasNext()) {
        i.next();
        c.loadValueFromXml(attr, i.key(), i.value());
    }

    return c;
}

QStringList RedisClient::ConnectionConfig::allowedNamespaces()
{
    return QStringList() << "*";
}

bool RedisClient::ConnectionConfig::loadValueFromXml(const QDomNamedNodeMap & attr,
                                                    const QString& name,
                                                    const QString& target)
{
    if (!attr.contains(name))
        return false;

    QString targetKey = (target.isEmpty())? name : target;

    QString rawValue = attr.namedItem(name).nodeValue();

    if (targetKey.contains("timeout") or targetKey.contains("port")) {
        m_parameters[targetKey] = rawValue.toInt();
    } else {
        m_parameters[targetKey] = rawValue;
    }

    return true;
}

QDomElement RedisClient::ConnectionConfig::toXml()
{
    QDomDocument dom;
    QDomElement xml = dom.createElement("connection");

    saveXmlAttribute(dom, xml, "name", param<QString>("name"));
    saveXmlAttribute(dom, xml, "host", param<QString>("host"));
    saveXmlAttribute(dom, xml, "port", QString::number(param<int>("port")));

    if (useAuth()) {
        saveXmlAttribute(dom, xml, "auth", param<QString>("auth"));
    }

    if (param<QString>("namespace_separator") != QString(DEFAULT_NAMESPACE_SEPARATOR)) {
        saveXmlAttribute(dom, xml, "namespaceSeparator", param<QString>("namespace_separator"));
    }

    saveXmlAttribute(dom, xml, "connectionTimeout", QString::number(param<int>("timeout_connect")));
    saveXmlAttribute(dom, xml, "executeTimeout", QString::number(param<int>("timeout_execute")));

    if (useSshTunnel()) {
        saveXmlAttribute(dom, xml, "sshHost", param<QString>("ssh_host"));
        saveXmlAttribute(dom, xml, "sshUser", param<QString>("ssh_user"));
        saveXmlAttribute(dom, xml, "sshPassword", param<QString>("ssh_password"));
        saveXmlAttribute(dom, xml, "sshPort", QString::number(param<int>("ssh_port")));
        saveXmlAttribute(dom, xml, "sshPrivateKey", param<QString>("ssh_private_key_path"));
    }

    return xml;
}

void RedisClient::ConnectionConfig::saveXmlAttribute(QDomDocument & document, QDomElement & root, const QString& name, const QString& value)
{
    QDomAttr attr = document.createAttribute(name);
    attr.setValue(value);
    root.setAttributeNode(attr);
}
