#pragma once
#include <qredisclient/connection.h>
#include <QSharedPointer>
#include <functional>

#include "app/models/connectionconf.h"
#include "bulk-operations/connections.h"
#include "connections-tree/model.h"
#include "treeoperations.h"

namespace ValueEditor {
class TabsModel;
}

class Events;

class ConnectionsManager : public ConnectionsTree::Model,
                           public BulkOperations::ConnectionsModel {
  Q_OBJECT

 public:
  ConnectionsManager(const QString& m_configPath,
                     QSharedPointer<Events> events);

  ~ConnectionsManager(void);

  Q_INVOKABLE void addNewConnection(const ServerConfig& config,
                                    bool saveToConfig = true);

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

 signals:
  void editConnection(ServerConfig config);

  void connectionAboutToBeEdited(QString name);

 protected:
  bool loadConnectionsConfigFromFile(const QString& config,
                                     bool saveChangesToFile = false);

 private:
  void createServerItemForConnection(
      QSharedPointer<RedisClient::Connection> connection,
      QSharedPointer<TreeOperations> treeModel);

 private:
  QString m_configPath;
  QList<QSharedPointer<RedisClient::Connection>> m_connections;
  QHash<QSharedPointer<RedisClient::Connection>,
        QSharedPointer<ConnectionsTree::TreeItem>>
      m_connectionMapping;
  QSharedPointer<Events> m_events;
};
