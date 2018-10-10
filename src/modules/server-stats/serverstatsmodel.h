#pragma once
#include "common/tabviewmodel.h"
#include "exception.h"

namespace ServerStats {

class Model : public TabModel {
  Q_OBJECT
  ADD_EXCEPTION

  Q_PROPERTY(QVariantMap serverInfo READ serverInfo NOTIFY serverInfoChanged)

  Q_PROPERTY(QVariant slowLog READ slowLog NOTIFY slowLogChanged)
  Q_PROPERTY(bool refreshSlowLog READ refreshSlowLog WRITE setRefreshSlowLog)

  Q_PROPERTY(QVariant clients READ clients NOTIFY clientsChanged)
  Q_PROPERTY(bool refreshClients READ refreshClients WRITE setRefreshClients)

 public:
  Model(QSharedPointer<RedisClient::Connection> connection, int dbIndex);

  ~Model();

  QString getName() const override;

  QVariantMap serverInfo();

  QVariant slowLog();

  QVariant clients();

  bool refreshSlowLog();
  void setRefreshSlowLog(bool v);

  bool refreshClients();
  void setRefreshClients(bool v);

 signals:
  void serverInfoChanged();
  void slowLogChanged();
  void clientsChanged();

 private:
  QTimer m_serverInfoUpdateTimer;
  QTimer m_slowLogUpdateTimer;
  QTimer m_clientsUpdateTimer;
  QVariantMap m_serverInfo;
  QVariant m_slowLog;
  QVariant m_clients;
};
}  // namespace ServerStats
