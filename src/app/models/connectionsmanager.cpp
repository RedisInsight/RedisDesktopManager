#include <QtXml>
#include <QDebug>
#include <QAbstractItemModel>
#include "modules/connections-tree/items/serveritem.h"
#include "modules/redisclient/connectionconfig.h"
#include "modules/value-editor/viewmodel.h"
#include "treeoperations.h"
#include "connectionsmanager.h"
#include "app/widgets/consoletabs.h"
#include "modules/value-editor/viewmodel.h"

ConnectionsManager::ConnectionsManager(const QString& configPath, ConsoleTabs& tabs,
                                       QSharedPointer<ValueEditor::ViewModel> values)
    : ConnectionsTree::Model(),
      m_configPath(configPath),      
      m_tabs(tabs),
      m_values(values)
{
    if (!configPath.isEmpty() && QFile::exists(configPath)) {
        loadConnectionsConfigFromFile(configPath);
    }
}


ConnectionsManager::~ConnectionsManager(void)
{
}

void ConnectionsManager::addNewConnection(const RedisClient::ConnectionConfig &config, bool saveToConfig)
{
    QSharedPointer<RedisClient::Connection> connection(new RedisClient::Connection(config, false));
    connection->config.setOwner(connection.toWeakRef());

    //add connection to internal container
    m_connections.push_back(connection);

    //add connection to view container
    using namespace ConnectionsTree;

    QSharedPointer<TreeOperations> treeModel(new TreeOperations(connection, m_tabs));

    QObject::connect(treeModel.data(), &TreeOperations::openValueTab,
                     m_values.data(), &ValueEditor::ViewModel::openTab);

    QSharedPointer<ServerItem> serverItem = QSharedPointer<ServerItem>(
                new ServerItem(config.name,
                               treeModel.dynamicCast<ConnectionsTree::Operations>(),
                               static_cast<ConnectionsTree::Model>(this))
                );

    QObject::connect(serverItem.data(), &ConnectionsTree::ServerItem::editActionRequested, this, [this, connection]() {
        qDebug() << "Edit connection";        
        m_tabs.closeAllTabsWithName(connection->getConfig().name);
        emit editConnection(connection->config);
    });

    QObject::connect(serverItem.data(), &ConnectionsTree::ServerItem::deleteActionRequested, this, [this, connection]() {        
        QSharedPointer<ConnectionsTree::ServerItem> serverItem = m_connectionMapping[connection].dynamicCast<ConnectionsTree::ServerItem>();

        if (!serverItem)
            return;

        qDebug() << "Remove row";
        m_tabs.closeAllTabsWithName(connection->getConfig().name);        
        m_connections.removeAll(connection);
        m_connectionMapping.remove(connection);
        removeRootItem(serverItem);
        saveConfig();
    });

    m_connectionMapping.insert(connection, serverItem);
    addRootItem(serverItem);

    if (saveToConfig)
        saveConfig();
}

void ConnectionsManager::updateConnection(const RedisClient::ConnectionConfig &config)
{
    if (!config.getOwner())
        return;

    QSharedPointer<RedisClient::Connection> connection = config.getOwner().toStrongRef();
    connection->setConnectionConfig(config);
    auto serverItem = m_connectionMapping[connection].dynamicCast<ConnectionsTree::ServerItem>();

    if (!serverItem)
        return;

    serverItem->setName(config.name);

    emit dataChanged(index(serverItem->row(), 0, QModelIndex()),
                     index(serverItem->row(), 0, QModelIndex()));
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

            addNewConnection(conf, false);
        }        
    }
    conf.close();

    if (saveChangesToFile)
        saveConfig();

    return true;
}

void ConnectionsManager::saveConfig()
{
    saveConnectionsConfigToFile(m_configPath);
}

bool ConnectionsManager::saveConnectionsConfigToFile(const QString& pathToFile)
{
    QDomDocument config;

    QDomProcessingInstruction xmlProcessingInstruction = config.createProcessingInstruction("xml", "version=\"1.0\"");
    config.appendChild(xmlProcessingInstruction);

    QDomElement connectionsItem = config.createElement("connections");

    config.appendChild(connectionsItem);

    for (auto c : m_connections) {
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
    return m_connections.length();
}
