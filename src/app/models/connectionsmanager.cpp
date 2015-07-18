#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QAbstractItemModel>
#include <easylogging++.h>
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

    //set logger
    QObject::connect(connection.data(), &RedisClient::Connection::log, this, [this](const QString& info){
        QString msg = QString("Connection: %1").arg(info);
        LOG(INFO) << msg.toStdString();
    });

    QObject::connect(connection.data(), &RedisClient::Connection::error, this, [this](const QString& error){
        QString msg = QString("Connection: %1").arg(error);
        LOG(ERROR) << msg.toStdString();
    });

    //add connection to view container
    using namespace ConnectionsTree;

    QSharedPointer<TreeOperations> treeModel(new TreeOperations(connection, m_tabs));

    QObject::connect(treeModel.data(), &TreeOperations::openValueTab,
                     m_values.data(), &ValueEditor::ViewModel::openTab);
    QObject::connect(treeModel.data(), &TreeOperations::newKeyDialog,
                     m_values.data(), &ValueEditor::ViewModel::openNewKeyDialog);

    QSharedPointer<ServerItem> serverItem = QSharedPointer<ServerItem>(
                new ServerItem(config.param<QString>("name"),
                               treeModel.dynamicCast<ConnectionsTree::Operations>(),
                               static_cast<ConnectionsTree::Model>(this))
                );

    QObject::connect(serverItem.data(), &ConnectionsTree::ServerItem::editActionRequested, this, [this, connection]() {       
        m_tabs.closeAllTabsWithName(connection->getConfig().param<QString>("name"));
        emit editConnection(connection->config);
    });

    QObject::connect(serverItem.data(), &ConnectionsTree::ServerItem::deleteActionRequested, this, [this, connection]() {        
        QSharedPointer<ConnectionsTree::ServerItem> serverItem = m_connectionMapping[connection].dynamicCast<ConnectionsTree::ServerItem>();

        if (!serverItem)
            return;

        m_tabs.closeAllTabsWithName(connection->getConfig().param<QString>("name"));
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
    saveConfig();
    auto serverItem = m_connectionMapping[connection].dynamicCast<ConnectionsTree::ServerItem>();

    if (!serverItem)
        return;

    serverItem->setName(config.param<QString>("name"));

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

    QByteArray data = conf.readAll();
    QJsonDocument jsonConfig = QJsonDocument::fromJson(data);

    if (jsonConfig.isEmpty())
        return true;


    if (!jsonConfig.isArray()) {
        // Show ERROR to USER
        return false;
    }

    QJsonArray connections = jsonConfig.array();

    using namespace RedisClient;

    for (QJsonValue connection : connections) {
        if (!connection.isObject())
            continue;

        ConnectionConfig conf = ConnectionConfig::fromJsonObject(connection.toObject());

        if (conf.isNull())
            continue;

        addNewConnection(conf, false);
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
    QJsonArray connections;
    for (auto c : m_connections) {
        connections.push_back(QJsonValue(c->getConfig().toJsonObject()));
    }

    QJsonDocument config(connections);
    QFile confFile(pathToFile);

    if (confFile.open(QIODevice::WriteOnly)) {
        QTextStream outStream(&confFile);
        outStream.setCodec("UTF-8");
        outStream << config.toJson();
        confFile.close();
        return true;
    }

    return false;
}

int ConnectionsManager::size()
{
    return m_connections.length();
}
