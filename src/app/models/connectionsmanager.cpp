#include <QtXml>
#include "modules/connections-tree/items/serveritem.h"
#include "modules/redisclient/connectionconfig.h"
#include "treeoperations.h"
#include "connectionsmanager.h"
#include "app/widgets/consoletabs.h"

ConnectionsManager::ConnectionsManager(const QString& configPath, ConsoleTabs& tabs)
    : configPath(configPath), connectionSettingsChanged(false), m_tabs(tabs)
{
    if (!configPath.isEmpty() && QFile::exists(configPath)) {
        loadConnectionsConfigFromFile(configPath);
    }
}


ConnectionsManager::~ConnectionsManager(void)
{
    if (connectionSettingsChanged) {
        saveConnectionsConfigToFile(configPath);
    }
}

void ConnectionsManager::addConnection(QSharedPointer<RedisClient::Connection> connection)
{
    //add connection to internal container
    connections.push_back(connection);

    //add connection to view container
    using namespace ConnectionsTree;

    QSharedPointer<TreeOperations> treeModel(new TreeOperations(connection, m_tabs));

    QSharedPointer<ServerItem> serverItem = QSharedPointer<ServerItem>(
                new ServerItem(connection->getConfig().name,
                               treeModel.dynamicCast<ConnectionsTree::Operations>(),
                               static_cast<ConnectionsTree::Model>(this))
                );

    addRootItem(serverItem);

    //mark settings as unsaved
    connectionSettingsChanged = true;
}


bool ConnectionsManager::importConnections(const QString &path)
{
    if (loadConnectionsConfigFromFile(path, true)) {
        return true;
    }

    return false;
}


bool ConnectionsManager::loadConnectionsConfigFromFile(const QString& config, bool saveChangesToFile)
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

            addConnection(QSharedPointer<RedisClient::Connection>(new RedisClient::Connection(conf, false)));
        }        
    }
    conf.close();

    if (!saveChangesToFile)
        connectionSettingsChanged = false;    

    return true;
}

bool ConnectionsManager::saveConnectionsConfigToFile(const QString& pathToFile)
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
