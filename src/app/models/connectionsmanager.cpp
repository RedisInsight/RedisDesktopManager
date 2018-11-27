#include <QAbstractItemModel>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "app/events.h"
#include "configmanager.h"
#include "connectionsmanager.h"
#include "modules/bulk-operations/bulkoperationsmanager.h"
#include "modules/connections-tree/items/serveritem.h"
#include "modules/value-editor/tabsmodel.h"

ConnectionsManager::ConnectionsManager(const QString& configPath,
                                       QSharedPointer<Events> events)
    : ConnectionsTree::Model(), m_configPath(configPath), m_events(events) {
  if (!configPath.isEmpty() && QFile::exists(configPath)) {
    loadConnectionsConfigFromFile(configPath);
  }

  connect(m_events.data(), &Events::createNewConnection, this,
          [this](RedisClient::ConnectionConfig config) {
            addNewConnection(config);
          });
  connect(this, &ConnectionsTree::Model::error, m_events.data(),
          &Events::error);
}

ConnectionsManager::~ConnectionsManager(void) {}

void ConnectionsManager::addNewConnection(const ServerConfig& config,
                                          bool saveToConfig) {
  // add connection to internal container
  QSharedPointer<RedisClient::Connection> connection(
      new RedisClient::Connection(config));
  ServerConfig conf = config;
  conf.setOwner(connection.toWeakRef());
  connection->setConnectionConfig(conf);
  m_connections.push_back(connection);

  // add connection to connection tree
  auto treeModel = QSharedPointer<TreeOperations>(
      new TreeOperations(connection->clone(), m_events));
  createServerItemForConnection(connection, treeModel);

  if (saveToConfig) saveConfig();
}

void ConnectionsManager::updateConnection(const ServerConfig& config) {
  if (!config.getOwner()) return addNewConnection(config);

  QSharedPointer<RedisClient::Connection> connection =
      config.getOwner().toStrongRef();
  connection->setConnectionConfig(config);
  saveConfig();
  auto serverItem = m_connectionMapping[connection]
                        .dynamicCast<ConnectionsTree::ServerItem>();

  if (!serverItem) return;

  serverItem->setName(config.name());
  auto operations = serverItem->getOperations().dynamicCast<TreeOperations>();

  if (!operations) return;

  operations->setConnection(connection->clone());

  emit dataChanged(index(serverItem->row(), 0, QModelIndex()),
                   index(serverItem->row(), 0, QModelIndex()));
}

bool ConnectionsManager::importConnections(const QString& path) {
  if (loadConnectionsConfigFromFile(path, true)) {
    return true;
  }
  return false;
}

bool ConnectionsManager::loadConnectionsConfigFromFile(const QString& config,
                                                       bool saveChangesToFile) {
  QJsonArray connections;

  QFile conf(config);

  if (!conf.open(QIODevice::ReadOnly)) return false;

  QByteArray data = conf.readAll();
  conf.close();

  QJsonDocument jsonConfig = QJsonDocument::fromJson(data);

  if (jsonConfig.isEmpty()) return true;

  if (!jsonConfig.isArray()) {
    return false;
  }

  connections = jsonConfig.array();

  for (QJsonValue connection : connections) {
    if (!connection.isObject()) continue;

    ServerConfig conf = ServerConfig::fromJsonObject(connection.toObject());

    if (conf.isNull()) continue;

    addNewConnection(conf, false);
  }

  if (saveChangesToFile) saveConfig();

  return true;
}

void ConnectionsManager::saveConfig() {
  saveConnectionsConfigToFile(m_configPath);
}

bool ConnectionsManager::saveConnectionsConfigToFile(
    const QString& pathToFile) {
  QJsonArray connections;
  for (auto c : m_connections) {
    connections.push_back(QJsonValue(c->getConfig().toJsonObject()));
  }

  return saveJsonArrayToFile(connections, pathToFile);
}

bool ConnectionsManager::testConnectionSettings(const ServerConfig& config) {
  RedisClient::Connection testConnection(config);
  m_events->registerLoggerForConnection(testConnection);

  try {
    return testConnection.connect();
  } catch (const RedisClient::Connection::Exception&) {
    return false;
  }
}

ServerConfig ConnectionsManager::createEmptyConfig() const {
  return ServerConfig();
}

int ConnectionsManager::size() { return m_connections.length(); }

QSharedPointer<RedisClient::Connection> ConnectionsManager::getByIndex(
    int index) {
  return m_connections[index];
}

QStringList ConnectionsManager::getConnections() {
  QStringList result;

  for (QSharedPointer<RedisClient::Connection> c : m_connections) {
    result.append(c->getConfig().name());
  }

  return result;
}

void ConnectionsManager::createServerItemForConnection(
    QSharedPointer<RedisClient::Connection> connection,
    QSharedPointer<TreeOperations> treeModel) {
  using namespace ConnectionsTree;
  QString name = connection->getConfig().name();
  auto serverItem = QSharedPointer<ServerItem>(
      new ServerItem(name, treeModel.dynamicCast<ConnectionsTree::Operations>(),
                     *static_cast<ConnectionsTree::Model*>(this)));

  QObject::connect(
      serverItem.data(), &ConnectionsTree::ServerItem::editActionRequested,
      this, [this, connection, name]() {
        emit connectionAboutToBeEdited(name);
        emit editConnection(static_cast<ServerConfig>(connection->getConfig()));
      });

  QObject::connect(
      serverItem.data(), &ConnectionsTree::ServerItem::deleteActionRequested,
      this, [this, connection, name]() {
        auto serverItem = m_connectionMapping[connection]
                              .dynamicCast<ConnectionsTree::ServerItem>();

        if (!serverItem) return;

        emit connectionAboutToBeEdited(name);
        m_connections.removeAll(connection);
        m_connectionMapping.remove(connection);
        removeRootItem(serverItem);
        saveConfig();
      });

  m_connectionMapping.insert(connection, serverItem);
  addRootItem(serverItem);
}
