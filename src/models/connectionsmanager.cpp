#include <QtXml>
#include "connections-tree/items/serveritem.h"
#include "connectionsmanager.h"
#include "core/connectionconfig.h"
#include "core/protocols/abstractprotocol.h"

ConnectionsManager::ConnectionsManager(const QString& configPath)
    : configPath(configPath), connectionSettingsChanged(false)
{
    if (!configPath.isEmpty() && QFile::exists(configPath)) {
        LoadConnectionsConfigFromFile(configPath);
    }
}


ConnectionsManager::~ConnectionsManager(void)
{
    if (connectionSettingsChanged) {
        SaveConnectionsConfigToFile(configPath);
    }
}

void ConnectionsManager::AddConnection(QSharedPointer<RedisClient::Connection> connection)
{
    //add connection to internal container
    connections.push_back(connection);

    //add connection to view container
    using namespace ConnectionsTree;

    QSharedPointer<RedisClient::AbstractProtocol> protocol = connection->operations();

    QSharedPointer<ServerItem> serverItem = QSharedPointer<ServerItem>(
                new ServerItem(connection->getConfig().name,
                               protocol.dynamicCast<ConnectionsTree::Operations>(),
                               static_cast<ConnectionsTree::Model>(this))
                );

    addRootItem(serverItem);

    //mark settings as unsaved
    connectionSettingsChanged = true;    
}


bool ConnectionsManager::ImportConnections(const QString &path)
{
    if (LoadConnectionsConfigFromFile(path, true)) {
        return true;
    }

    return false;
}


bool ConnectionsManager::LoadConnectionsConfigFromFile(const QString& config, bool saveChangesToFile)
{
    QFile conf(config);
    
    if (!conf.open(QIODevice::ReadOnly)) 
        return false;    
    
    QDomDocument xmlConf;

    if (xmlConf.setContent(&conf)) {

        QDomNodeList connectionsList = xmlConf.elementsByTagName("connection");
        
        for (int i = 0; i < connectionsList.size(); ++i) {

            QDomNode connection = connectionsList.at(i);

            if (connection.nodeName() != "connection") continue;

            RedisClient::ConnectionConfig conf = RedisClient::ConnectionConfig::createFromXml(connection);

            if (conf.isNull()) continue;

            AddConnection(QSharedPointer<RedisClient::Connection>(new RedisClient::Connection(conf, false)));
        }        
    }
    conf.close();

    if (!saveChangesToFile)
        connectionSettingsChanged = false;    

    return true;
}

bool ConnectionsManager::SaveConnectionsConfigToFile(const QString& pathToFile)
{
    QDomDocument config;

    QDomProcessingInstruction xmlProcessingInstruction = config.createProcessingInstruction("xml", "version=\"1.0\"");
    config.appendChild(xmlProcessingInstruction);

    QDomElement connectionsItem = config.createElement("connections");

    config.appendChild(connectionsItem);

    for (auto c : connections) {
        connectionsItem.appendChild(c->getConfig().toXml(config));
    }

    QFile confFile(pathToFile);

    if (confFile.open(QIODevice::WriteOnly)) {
        QTextStream(&confFile) << config.toString();
        confFile.close();
        return true;
    }

    return false;
}

int ConnectionsManager::size()
{
    return connections.length();
}
