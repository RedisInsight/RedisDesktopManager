#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QAbstractItemModel>
#include <easylogging++.h>

#include "modules/bulk-operations/bulkoperationsmanager.h"
#include "modules/connections-tree/items/serveritem.h"
#include "modules/value-editor/tabsmodel.h"
#include "connectionsmanager.h"
#include "configmanager.h"

void registerLogger(const RedisClient::Connection& connection)
{
    QObject::connect(&connection, &RedisClient::Connection::log, [](const QString& info){
        QString msg = QString("Connection: %1").arg(info);
        LOG(INFO) << msg.toStdString();
    });

    QObject::connect(&connection, &RedisClient::Connection::error, [](const QString& error){
        QString msg = QString("Connection: %1").arg(error);
        LOG(ERROR) << msg.toStdString();
    });
}

QSharedPointer<RedisClient::Connection> cloneConnection(QSharedPointer<RedisClient::Connection> c)
{
    RedisClient::ConnectionConfig config = c->getConfig();
    auto copy = QSharedPointer<RedisClient::Connection>(new RedisClient::Connection(config));

    registerLogger(*copy.data());

    return copy;
}

ConnectionsManager::ConnectionsManager(const QString& configPath)
    : ConnectionsTree::Model(),
      m_configPath(configPath)
{
    if (!configPath.isEmpty() && QFile::exists(configPath)) {
        loadConnectionsConfigFromFile(configPath);
    }
}

ConnectionsManager::~ConnectionsManager(void)
{
}

void ConnectionsManager::addNewConnection(const ServerConfig &config, bool saveToConfig)
{
    //add connection to internal container
    QSharedPointer<RedisClient::Connection> connection(new RedisClient::Connection(config));
    ServerConfig conf = config;
    conf.setOwner(connection.toWeakRef());
    connection->setConnectionConfig(conf);
    m_connections.push_back(connection);

    //add connection to connection tree
    auto treeModel = createTreeModelForConnection(connection);
    createServerItemForConnection(connection, treeModel);

    if (saveToConfig) saveConfig();
}

void ConnectionsManager::updateConnection(const ServerConfig &config)
{
    if (!config.getOwner())
        return addNewConnection(config);

    QSharedPointer<RedisClient::Connection> connection = config.getOwner().toStrongRef();
    connection->setConnectionConfig(config);
    saveConfig();
    auto serverItem = m_connectionMapping[connection].dynamicCast<ConnectionsTree::ServerItem>();

    if (!serverItem)
        return;

    serverItem->setName(config.name());
    auto operations = serverItem->getOperations().dynamicCast<TreeOperations>();

    if (!operations)
        return;

    operations->setConnection(cloneConnection(connection));

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

        ServerConfig conf = ServerConfig::fromJsonObject(connection.toObject());

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

bool ConnectionsManager::testConnectionSettings(const ServerConfig &config)
{
    RedisClient::Connection testConnection(config);
    registerLogger(testConnection);

    try {
        return testConnection.connect();
    } catch (const RedisClient::Connection::Exception&) {
        return false;
    }
}

ServerConfig ConnectionsManager::createEmptyConfig() const
{
    return ServerConfig();
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

QSharedPointer<TreeOperations> ConnectionsManager::createTreeModelForConnection(
        QSharedPointer<RedisClient::Connection> connection)
{
    QSharedPointer<TreeOperations> treeModel(
                new TreeOperations(cloneConnection(connection), *this));
    return treeModel;
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
        emit connectionAboutToBeEdited(name);
        emit editConnection(static_cast<ServerConfig>(connection->getConfig()));
    });

    QObject::connect(serverItem.data(), &ConnectionsTree::ServerItem::deleteActionRequested,
                     this, [this, connection, name]()
    {
        auto serverItem = m_connectionMapping[connection].dynamicCast<ConnectionsTree::ServerItem>();

        if (!serverItem)
            return;

        emit connectionAboutToBeEdited(name);
        m_connections.removeAll(connection);
        m_connectionMapping.remove(connection);
        removeRootItem(serverItem);
        saveConfig();
    });

    m_connectionMapping.insert(connection, serverItem);
    addRootItem(serverItem);
}
