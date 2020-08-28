#include "connectionsmanager.h"

#include <QAbstractItemModel>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "app/events.h"
#include "configmanager.h"
#include "modules/bulk-operations/bulkoperationsmanager.h"
#include "modules/connections-tree/items/serveritem.h"
#include "modules/value-editor/tabsmodel.h"

ConnectionsManager::ConnectionsManager(const QString& configPath,
                                       QSharedPointer<Events> events)
    : ConnectionsTree::Model(), m_configPath(configPath), m_events(events) {
  if (!configPath.isEmpty() && QFile::exists(configPath)) {
    loadConnectionsConfigFromFile(configPath);
  }

  connect(this, &ConnectionsTree::Model::error, m_events.data(),
          &Events::error);
}

ConnectionsManager::~ConnectionsManager(void) {}

void ConnectionsManager::addNewConnection(
    const ServerConfig& config, bool saveToConfig,
    QSharedPointer<ConnectionsTree::ServerGroup> group) {
  createServerItemForConnection(config, group);

  if (saveToConfig) saveConfig();

  buildConnectionsCache();
}

void ConnectionsManager::addNewGroup(const QString& name) {
  auto group = QSharedPointer<ConnectionsTree::ServerGroup>(
      new ConnectionsTree::ServerGroup(
          name, *static_cast<ConnectionsTree::Model*>(this)));

  addGroup(group);

  saveConfig();
}

void ConnectionsManager::updateGroup(const ConnectionGroup &group)
{
    auto serverGroup = group.serverGroup();

    if (!serverGroup){
        qWarning() << "invalid server group";
        return;
    }

    onItemChanged(serverGroup);

    saveConfig();

    buildConnectionsCache();
}

void ConnectionsManager::updateConnection(const ServerConfig& config) {
  if (!config.owner()) return addNewConnection(config);

  auto treeOperations = config.owner().toStrongRef();

  if (!treeOperations) return;

  treeOperations->setConfig(config);

  saveConfig();
}

bool ConnectionsManager::importConnections(const QString& path) {
  if (loadConnectionsConfigFromFile(path, true)) {
    emit sizeChanged();
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

    auto obj = connection.toObject();

    if (obj.contains("type") && obj.contains("connections") &&
        obj.contains("name") && obj["connections"].isArray() &&
        obj["type"].toString().toLower() == "group") {
      auto groupConnections = obj["connections"].toArray();

      auto group = QSharedPointer<ConnectionsTree::ServerGroup>(
          new ConnectionsTree::ServerGroup(
              obj["name"].toString(),
              *static_cast<ConnectionsTree::Model*>(this)));

      for (QJsonValue c : groupConnections) {
        if (!c.isObject()) continue;

        ServerConfig conf(c.toObject().toVariantHash());

        if (conf.isNull()) continue;

        addNewConnection(conf, false, group);
      }

      addGroup(group);
    }

    ServerConfig conf(obj.toVariantHash());

    if (conf.isNull()) continue;

    addNewConnection(conf, false);
  }

  if (saveChangesToFile) saveConfig();

  buildConnectionsCache();

  return true;
}

void ConnectionsManager::saveConfig() {
  saveConnectionsConfigToFile(m_configPath);
}

bool ConnectionsManager::saveConnectionsConfigToFile(
    const QString& pathToFile) {
  QJsonArray connections;

  auto addConfig = [](QSharedPointer<ConnectionsTree::TreeItem> i,
                      QJsonArray& connections) {
    auto srvItem = i.dynamicCast<ConnectionsTree::ServerItem>();

    if (!srvItem) return;

    auto op = srvItem->getOperations().dynamicCast<TreeOperations>();

    if (!op) return;

    connections.push_back(QJsonValue(op->config().toJsonObject()));
  };

  for (auto item : m_treeItems) {
    if (item->type() == "server_group") {
      QJsonObject group;
      group["type"] = "group";
      group["name"] = item->getDisplayName();

      QJsonArray groupConnections;

      for (auto srv : item->getAllChilds()) {
        addConfig(srv, groupConnections);
      }

      group["connections"] = groupConnections;
      connections.push_back(QJsonValue(group));

    } else if (item->type() == "server") {
      addConfig(item, connections);
    }
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

int ConnectionsManager::size() {
  int connectionsCount = 0;

  for (auto item : m_treeItems) {
    if (item->type() == "server_group") {
      connectionsCount += item->childCount();
    } else if (item->type() == "server") {
      connectionsCount++;
    }
  }
  return connectionsCount;
}

QSharedPointer<RedisClient::Connection> ConnectionsManager::getByIndex(
    int index) {
  auto op = m_connectionsCache.values().at(index)->getOperations();

  if (!op) return QSharedPointer<RedisClient::Connection>();

  auto treeOp = op.dynamicCast<TreeOperations>();

  if (!treeOp) return QSharedPointer<RedisClient::Connection>();

  return treeOp->connection();
}

QStringList ConnectionsManager::getConnections() {
  return m_connectionsCache.keys();
}

void ConnectionsManager::applyGroupChanges() {
  ConnectionsTree::Model::applyGroupChanges();

  buildConnectionsCache();

  saveConfig();
}

void ConnectionsManager::createServerItemForConnection(
    const ServerConfig& config,
    QSharedPointer<ConnectionsTree::ServerGroup> group) {
  using namespace ConnectionsTree;

  auto treeModel =
      QSharedPointer<TreeOperations>(new TreeOperations(config, m_events));

  connect(treeModel.data(), &TreeOperations::createNewConnection, this,
          [this](const ServerConfig& config) { addNewConnection(config); });

  QWeakPointer<TreeItem> parent;

  if (group) {
    parent = group.toWeakRef();
  }

  auto serverItem = QSharedPointer<ServerItem>(
      new ServerItem(treeModel.dynamicCast<ConnectionsTree::Operations>(),
                     *static_cast<ConnectionsTree::Model*>(this), parent));
  serverItem->setWeakPointer(serverItem.toWeakRef());

  connect(
      treeModel.data(), &TreeOperations::configUpdated, this,
      [this, serverItem]() {
        if (!serverItem) return;

        onItemChanged(
            serverItem.dynamicCast<ConnectionsTree::TreeItem>().toWeakRef());
      });

  connect(serverItem.data(), &ConnectionsTree::ServerItem::editActionRequested,
          this, [this, treeModel]() {
            if (!treeModel) return;

            emit connectionAboutToBeEdited(treeModel->config().name());            

            emit editConnection(treeModel->config());
          });

  connect(serverItem.data(),
          &ConnectionsTree::ServerItem::deleteActionRequested, this,
          [this, serverItem, treeModel, group]() {
            if (!serverItem || !treeModel) return;

            emit connectionAboutToBeEdited(treeModel->config().name());

            if (group) {
              group->removeChild(serverItem);
            } else {
              removeRootItem(serverItem);
            }

            buildConnectionsCache();

            emit sizeChanged();
            saveConfig();
          });

  if (group) {
    group->addServer(serverItem);
  } else {
    addRootItem(serverItem);
  }
}

void ConnectionsManager::addGroup(
    QSharedPointer<ConnectionsTree::ServerGroup> group) {
  connect(group.data(), &ConnectionsTree::ServerGroup::editActionRequested,
          this, [this, group]() {
            if (!group) return;

            ConnectionGroup g(group);

            emit editConnectionGroup(g);
          });

  connect(group.data(), &ConnectionsTree::ServerGroup::deleteActionRequested,
          this, [this, group]() {
            if (!group) return;

            removeRootItem(group);

            buildConnectionsCache();

            emit sizeChanged();
            saveConfig();
          });

  addRootItem(group);

  buildConnectionsCache();
}

void ConnectionsManager::buildConnectionsCache() {
  m_connectionsCache.clear();

  for (auto item : m_treeItems) {
    if (item->type() == "server_group") {
      QString nameTemplate = QString("[%1] %2").arg(item->getDisplayName());

      for (auto srv : item->getAllChilds()) {
        QString name = nameTemplate.arg(srv->getDisplayName());
        m_connectionsCache[name] =
            srv.dynamicCast<ConnectionsTree::ServerItem>();
      }
    } else if (item->type() == "server") {
      m_connectionsCache[item->getDisplayName()] =
          item.dynamicCast<ConnectionsTree::ServerItem>();
    }
  }
}
