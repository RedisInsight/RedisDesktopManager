#include "serverstatsmodel.h"
#include <qredisclient/redisclient.h>
#include <QCoreApplication>

ServerStats::Model::Model(QSharedPointer<RedisClient::Connection> connection,
                          int dbIndex, QList<QByteArray>)
    : TabModel(connection, dbIndex) {
  m_serverInfoUpdateTimer.setInterval(5000);
  m_serverInfoUpdateTimer.setSingleShot(false);
  m_slowLogUpdateTimer.setInterval(5000);
  m_slowLogUpdateTimer.setSingleShot(false);
  m_clientsUpdateTimer.setInterval(5000);
  m_clientsUpdateTimer.setSingleShot(false);

  m_pubSubMonitorConnection = connection->clone();
  setRefreshPubSubMonitor(true);

  QObject::connect(&m_serverInfoUpdateTimer, &QTimer::timeout, this, [this] {
    m_connection->cmd({"INFO", "all"}, this, 0,
                      [this](RedisClient::Response r) {
                        m_serverInfo =
                            RedisClient::ServerInfo::fromString(
                                QString::fromUtf8(r.value().toByteArray()))
                                .parsed.toVariantMap();
                        emit serverInfoChanged();
                      },
                      [this](const QString& e) { cmdErrorHander(e); });
  });

  QObject::connect(&m_slowLogUpdateTimer, &QTimer::timeout, this, [this] {
    m_connection->cmd({"SLOWLOG", "GET", "15"}, this, 0,
                      [this](RedisClient::Response r) {
                        QVariantList processed;

                        for (QVariant item : r.value().toList()) {
                          auto itemList = item.toList();
                          QVariantMap row;
                          row.insert("time", itemList[1]);
                          row.insert("exec_time", itemList[2]);
                          row.insert("cmd", itemList[3]);
                          processed.append(row);
                        }

                        m_slowLog = processed;
                        emit slowLogChanged();
                      },
                      [this](const QString& e) { cmdErrorHander(e); });
  });

  QObject::connect(&m_clientsUpdateTimer, &QTimer::timeout, this, [this] {
    m_connection->cmd({"CLIENT", "LIST"}, this, 0,
                      [this](RedisClient::Response r) {
                        QVariant result = r.value();
                        QStringList lines = result.toString().split("\r\n");

                        QVariantList parsedClients;

                        for (auto rawLine : lines) {
                          QStringList lineParts = rawLine.split(" ");
                          QVariantMap parsed;

                          for (auto linePart : lineParts) {
                            QStringList keyAndVal = linePart.split("=");

                            if (keyAndVal.size() > 1) {
                              parsed.insert(keyAndVal[0], keyAndVal[1]);
                            } else {
                              parsed.insert(keyAndVal[0], "");
                            }
                          }
                          parsedClients.append(parsed);
                        }

                        m_clients = parsedClients;
                        emit clientsChanged();
                      },
                      [this](const QString& e) { cmdErrorHander(e); });
  });

  QObject::connect(this, &TabModel::initialized, [this]() {
    m_serverInfoUpdateTimer.start();
    m_slowLogUpdateTimer.start();
    m_clientsUpdateTimer.start();
  });
}

ServerStats::Model::~Model() {
  m_serverInfoUpdateTimer.stop();
  m_slowLogUpdateTimer.stop();
  m_clientsUpdateTimer.stop();
}

QString ServerStats::Model::getName() const {
  return QCoreApplication::translate("RDM", "Server %0")
      .arg(m_connection->getConfig().name());
}

QVariantMap ServerStats::Model::serverInfo() { return m_serverInfo; }

QVariant ServerStats::Model::slowLog() { return m_slowLog; }

QVariant ServerStats::Model::clients() { return m_clients; }

QVariant ServerStats::Model::pubSubChannels() {
  QVariantList r;
  for (QByteArray ch : m_pubSubChannels) {
    r.append(QVariant(ch));
  }
  return r;
}

bool ServerStats::Model::refreshSlowLog() {
  return m_slowLogUpdateTimer.isActive();
}

void ServerStats::Model::setRefreshSlowLog(bool v) {
  if (refreshSlowLog() != v && refreshSlowLog()) m_slowLogUpdateTimer.stop();
  if (refreshSlowLog() != v && !refreshSlowLog()) m_slowLogUpdateTimer.start();
}

bool ServerStats::Model::refreshClients() {
  return m_clientsUpdateTimer.isActive();
}

void ServerStats::Model::setRefreshClients(bool v) {
  if (refreshClients() != v && refreshClients()) m_clientsUpdateTimer.stop();
  if (refreshClients() != v && !refreshClients()) m_clientsUpdateTimer.start();
}

bool ServerStats::Model::refreshPubSubMonitor() {
  return m_pubSubMonitorConnection->isConnected();
}

void ServerStats::Model::setRefreshPubSubMonitor(bool v) {
  if (m_pubSubMonitorConnection->isConnected() && !v) {
    m_pubSubMonitorConnection->disconnect();
    return;
  }

  if (!m_pubSubMonitorConnection->isConnected() && v) {
    m_pubSubMonitorConnection->cmd(
        {"PSUBSCRIBE", "*"}, this, 0,
        [this](RedisClient::Response result) {
          if (result.type() != RedisClient::Response::Array) {
            return;
          }

          QVariantList msg = result.value().toList();

          if (msg.size() == 4) {
            m_pubSubChannels.insert(msg[2].toByteArray());
            emit pubSubChannelsChanged();
          }
        },
        [this](const QString& e) { cmdErrorHander(e); });
  }
}

void ServerStats::Model::subscribeToChannel(const QString &c)
{
    emit openConsoleTerminal(m_connection, m_dbIndex, {"SUBSCRIBE", c.toUtf8()});
}

void ServerStats::Model::cmdErrorHander(const QString& err) { emit error(err); }
