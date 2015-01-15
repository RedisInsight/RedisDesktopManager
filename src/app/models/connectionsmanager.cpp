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
      m_connectionSettingsChanged(false),
      m_tabs(tabs),
      m_values(values)
{
    if (!configPath.isEmpty() && QFile::exists(configPath)) {
        loadConnectionsConfigFromFile(configPath);
    }
}


ConnectionsManager::~ConnectionsManager(void)
{
    if (m_connectionSettingsChanged) {
        saveConnectionsConfigToFile(m_configPath);
    }
}

void ConnectionsManager::addNewConnection(const RedisClient::ConnectionConfig &config)
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
        emit editConnection(connection->config);
    });

    m_connectionMapping.insert(connection, serverItem);
    addRootItem(serverItem);

    //mark settings as unsaved
    m_connectionSettingsChanged = true;
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

            addNewConnection(conf);
        }        
    }
    conf.close();

    if (!saveChangesToFile)
        m_connectionSettingsChanged = false;

    return true;
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
