#pragma once
#include <qredisclient/connection.h>

#include <QSharedPointer>
#include <functional>

#include "app/models/connectionconf.h"
#include "connections-tree/items/servergroup.h"
#include "bulk-operations/connections.h"
#include "connections-tree/model.h"
#include "treeoperations.h"
#include "connectiongroup.h"

namespace ValueEditor {
class TabsModel;
}

class Events;

class ConnectionsManager : public ConnectionsTree::Model,
                           public BulkOperations::ConnectionsModel {
  Q_OBJECT

  Q_PROPERTY(int connectionsCount READ size NOTIFY sizeChanged)

 public:
  ConnectionsManager(const QString& m_configPath,
                     QSharedPointer<Events> events);

  ~ConnectionsManager(void);

  Q_INVOKABLE void addNewConnection(const ServerConfig& config,
                                    bool saveToConfig = true,
                                    QSharedPointer<ConnectionsTree::ServerGroup> group =
                                        QSharedPointer<ConnectionsTree::ServerGroup>());

  Q_INVOKABLE void addNewGroup(const QString& name);

  Q_INVOKABLE void updateGroup(const ConnectionGroup& group);

  Q_INVOKABLE void updateConnection(const ServerConfig& config);

  Q_INVOKABLE bool importConnections(const QString&);

  Q_INVOKABLE bool saveConnectionsConfigToFile(const QString&);

  Q_INVOKABLE bool testConnectionSettings(const ServerConfig& config);

  Q_INVOKABLE ServerConfig createEmptyConfig() const;

  void saveConfig();

  Q_INVOKABLE int size();

  // BulkOperations model methods
  QSharedPointer<RedisClient::Connection> getByIndex(int index) override;

  QStringList getConnections() override;

  void applyGroupChanges() override;

 signals:
  void editConnection(ServerConfig config);

  void editConnectionGroup(ConnectionGroup group);

  void connectionAboutToBeEdited(QString name);

  void sizeChanged();

 protected:
  bool loadConnectionsConfigFromFile(const QString& config,
                                     bool saveChangesToFile = false);

 private:
  void createServerItemForConnection(const ServerConfig &config,
      QSharedPointer<ConnectionsTree::ServerGroup> group=QSharedPointer<ConnectionsTree::ServerGroup>());

  void addGroup(QSharedPointer<ConnectionsTree::ServerGroup> serverGroup);

  void buildConnectionsCache();

 private:
  QString m_configPath;
  QSharedPointer<Events> m_events;
  QMap<QString, QSharedPointer<ConnectionsTree::ServerItem>> m_connectionsCache;
};
