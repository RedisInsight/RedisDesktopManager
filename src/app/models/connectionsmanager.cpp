#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QAbstractItemModel>
#include <easylogging++.h>

#include "modules/bulk-operations/bulkoperationsmanager.h"
#include "modules/connections-tree/items/serveritem.h"
#include "modules/value-editor/viewmodel.h"
#include "app/widgets/consoletabs.h"
#include "connectionsmanager.h"
#include "configmanager.h"

ConnectionsManager::ConnectionsManager(const QString& configPath, ConsoleTabs& tabs)
    : ConnectionsTree::Model(),
      m_configPath(configPath),      
      m_consoleTabs(tabs)
{
    if (!configPath.isEmpty() && QFile::exists(configPath)) {
        loadConnectionsConfigFromFile(configPath);
    }
}

ConnectionsManager::~ConnectionsManager(void)
{
}

void ConnectionsManager::addNewConnection(const ConnectionConfig &config, bool saveToConfig)
{
    //add connection to internal container
    QSharedPointer<RedisClient::Connection> connection(new RedisClient::Connection(config));
    ConnectionConfig conf = config;
    conf.setOwner(connection.toWeakRef());
    connection->setConnectionConfig(conf);
    m_connections.push_back(connection);

    //set loggers
    registerLogger(connection);

    //add connection to connection tree
    auto treeModel = createTreeModelForConnection(connection);
    createServerItemForConnection(connection, treeModel);

    if (saveToConfig) saveConfig();
}

void ConnectionsManager::updateConnection(const ConnectionConfig &config)
{
    if (!config.getOwner())
        return;

    QSharedPointer<RedisClient::Connection> connection = config.getOwner().toStrongRef();
    connection->setConnectionConfig(config);
    saveConfig();
    auto serverItem = m_connectionMapping[connection].dynamicCast<ConnectionsTree::ServerItem>();

    if (!serverItem)
        return;

    serverItem->setName(config.name());

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
    QJsonArray connections;

    if (config.endsWith(".xml")) {
        connections = ConfigManager::xmlConfigToJsonArray(config);
    } else {
        QFile conf(config);

        if (!conf.open(QIODevice::ReadOnly))
            return false;

        QByteArray data = conf.readAll();
        conf.close();

        QJsonDocument jsonConfig = QJsonDocument::fromJson(data);

        if (jsonConfig.isEmpty())
            return true;

        if (!jsonConfig.isArray()) {
            return false;
        }

        connections = jsonConfig.array();
    }

    for (QJsonValue connection : connections) {
        if (!connection.isObject())
            continue;

        ConnectionConfig conf = ConnectionConfig::fromJsonObject(connection.toObject());

        if (conf.isNull())
            continue;

        addNewConnection(conf, false);
    }

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

    return saveJsonArrayToFile(connections, pathToFile);
}

int ConnectionsManager::size()
{
    return m_connections.length();
}

QSharedPointer<RedisClient::Connection> ConnectionsManager::getByIndex(int index)
{
    return m_connections[index];
}

QStringList ConnectionsManager::getConnections()
{
    QStringList result;

    for (QSharedPointer<RedisClient::Connection> c : m_connections) {
        result.append(c->getConfig().name());
    }

    return result;
}

QSharedPointer<TreeOperations> ConnectionsManager::createTreeModelForConnection(QSharedPointer<RedisClient::Connection> connection)
{
    QSharedPointer<TreeOperations> treeModel(new TreeOperations(connection, m_consoleTabs, *this));
    return treeModel;
}

void ConnectionsManager::registerLogger(QSharedPointer<RedisClient::Connection> connection)
{
    QObject::connect(connection.data(), &RedisClient::Connection::log, this, [this](const QString& info){
        QString msg = QString("Connection: %1").arg(info);
        LOG(INFO) << msg.toStdString();
    });

    QObject::connect(connection.data(), &RedisClient::Connection::error, this, [this](const QString& error){
        QString msg = QString("Connection: %1").arg(error);
        LOG(ERROR) << msg.toStdString();
    });
}

void ConnectionsManager::createServerItemForConnection(QSharedPointer<RedisClient::Connection> connection,
                                                       QSharedPointer<TreeOperations> treeModel)
{
    using namespace ConnectionsTree;
    QString name = connection->getConfig().name();
    auto serverItem = QSharedPointer<ServerItem>(
                new ServerItem(name,
                               treeModel.dynamicCast<ConnectionsTree::Operations>(),
                               *static_cast<ConnectionsTree::Model*>(this)));

    QObject::connect(serverItem.data(), &ConnectionsTree::ServerItem::editActionRequested,
                     this, [this, connection, name]()
    {
        m_consoleTabs.closeAllTabsWithName(name);
        emit editConnection(static_cast<ConnectionConfig>(connection->getConfig()));
    });

    QObject::connect(serverItem.data(), &ConnectionsTree::ServerItem::deleteActionRequested,
                     this, [this, connection, name]()
    {
        auto serverItem = m_connectionMapping[connection].dynamicCast<ConnectionsTree::ServerItem>();

        if (!serverItem)
            return;

        m_consoleTabs.closeAllTabsWithName(name);
        m_connections.removeAll(connection);
        m_connectionMapping.remove(connection);
        removeRootItem(serverItem);
        saveConfig();
    });

    m_connectionMapping.insert(connection, serverItem);
    addRootItem(serverItem);
}
