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

  QVariantMap serverInfo() const;

  QVariant slowLog() const;

  QVariant clients() const;

  QVariant pubSubChannels() const;

  bool refreshSlowLog() const;
  void setRefreshSlowLog(bool v);

  bool refreshClients() const;
  void setRefreshClients(bool v);

  bool refreshPubSubMonitor() const;
  void setRefreshPubSubMonitor(bool v);

  Q_INVOKABLE void subscribeToChannel(const QString& c);
  Q_INVOKABLE void monitorCommands();
  Q_INVOKABLE void openTerminal();

 signals:
  void serverInfoChanged();
  void slowLogChanged();
  void clientsChanged();
  void pubSubChannelsChanged();
  void openConsoleTerminal(QSharedPointer<RedisClient::Connection> c,
                           int db, bool inNewTab, QList<QByteArray> cmd);

 protected:
  void cmdErrorHander(const QString& err);

 protected slots:
  void srvInfoCallback();

  void slowLogCallback();

  void clientsCallback();

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
