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

  Q_PROPERTY(
      QVariant pubSubChannels READ pubSubChannels NOTIFY pubSubChannelsChanged)
  Q_PROPERTY(bool refreshPubSubMonitor READ refreshPubSubMonitor WRITE
                 setRefreshPubSubMonitor)

 public:
  Model(QSharedPointer<RedisClient::Connection> connection, int dbIndex, QList<QByteArray>);

  ~Model() override;

  QString getName() const override;

  QVariantMap serverInfo();

  QVariant slowLog();

  QVariant clients();

  QVariant pubSubChannels();

  bool refreshSlowLog();
  void setRefreshSlowLog(bool v);

  bool refreshClients();
  void setRefreshClients(bool v);

  bool refreshPubSubMonitor();
  void setRefreshPubSubMonitor(bool v);

  Q_INVOKABLE void subscribeToChannel(const QString& c);

 signals:
  void serverInfoChanged();
  void slowLogChanged();
  void clientsChanged();
  void pubSubChannelsChanged();
  void openConsoleTerminal(QSharedPointer<RedisClient::Connection> c,
                           int db, QList<QByteArray> cmd);

 protected:
  void cmdErrorHander(const QString& err);

 private:
  QTimer m_serverInfoUpdateTimer;
  QTimer m_slowLogUpdateTimer;
  QTimer m_clientsUpdateTimer;
  QSharedPointer<RedisClient::Connection> m_pubSubMonitorConnection;
  QVariantMap m_serverInfo;
  QVariant m_slowLog;
  QVariant m_clients;
  QSet<QByteArray> m_pubSubChannels;
};
}  // namespace ServerStats
