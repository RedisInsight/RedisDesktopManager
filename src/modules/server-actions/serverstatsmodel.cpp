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

  QObject::connect(&m_serverInfoUpdateTimer, &QTimer::timeout, this, &Model::srvInfoCallback);

  QObject::connect(&m_slowLogUpdateTimer, &QTimer::timeout, this, &Model::slowLogCallback);

  QObject::connect(&m_clientsUpdateTimer, &QTimer::timeout, this, &Model::clientsCallback);

  QObject::connect(this, &TabModel::initialized, [this]() {
    srvInfoCallback();
    m_serverInfoUpdateTimer.start();
  });
}

ServerStats::Model::~Model() {
  m_serverInfoUpdateTimer.stop();
  m_slowLogUpdateTimer.stop();
  m_clientsUpdateTimer.stop();
}

QString ServerStats::Model::getName() const {
  return QCoreApplication::translate("RESP", "Server %0")
      .arg(m_connection->getConfig().name());
}

QVariantMap ServerStats::Model::serverInfo() const { return m_serverInfo; }

QVariant ServerStats::Model::slowLog() const { return m_slowLog; }

QVariant ServerStats::Model::clients() const { return m_clients; }

QVariant ServerStats::Model::pubSubChannels() const {
  QVariantList r;
  for (QByteArray ch : m_pubSubChannels) {
    r.append(QVariant(ch));
  }
  return r;
}

bool ServerStats::Model::refreshSlowLog() const {
  return m_slowLogUpdateTimer.isActive();
}

void ServerStats::Model::setRefreshSlowLog(bool v) {
  if (refreshSlowLog() != v && refreshSlowLog()) m_slowLogUpdateTimer.stop();
  if (refreshSlowLog() != v && !refreshSlowLog()) {
      slowLogCallback();
      m_slowLogUpdateTimer.start();
  }
}

bool ServerStats::Model::refreshClients() const {
  return m_clientsUpdateTimer.isActive();
}

void ServerStats::Model::setRefreshClients(bool v) {
  if (refreshClients() != v && refreshClients()) m_clientsUpdateTimer.stop();
  if (refreshClients() != v && !refreshClients()) {
      clientsCallback();
      m_clientsUpdateTimer.start();
  }
}

bool ServerStats::Model::refreshPubSubMonitor() const {
  return m_pubSubMonitorConnection->isConnected();
}

void ServerStats::Model::setRefreshPubSubMonitor(bool v) {
  if (m_pubSubMonitorConnection->isConnected() && !v) {
    m_pubSubMonitorConnection->disconnect();
    return;
  }

  if (!m_pubSubMonitorConnection->isConnected() && v) {
    m_pubSubMonitorConnection->cmd(
        {"PSUBSCRIBE", "*"}, this, -1,
        [this](const RedisClient::Response& result) {
          if (result.type() != RedisClient::Response::Array) {
            return;
          }

          QVariantList msg = result.value().toList();

          if (msg.size() == 4) {
            m_pubSubChannels.insert(msg[2].toByteArray());            
          }
          emit pubSubChannelsChanged();
        },
        [this](const QString& e) { cmdErrorHander(e); });
  }
}

void ServerStats::Model::subscribeToChannel(const QString &c)
{
    emit openConsoleTerminal(m_connection, m_dbIndex, true, {"SUBSCRIBE", c.toUtf8()});
}

void ServerStats::Model::monitorCommands()
{
    emit openConsoleTerminal(m_connection, m_dbIndex, true, {"MONITOR"});
}

void ServerStats::Model::openTerminal()
{
    emit openConsoleTerminal(m_connection, m_dbIndex, true, {});
}

void ServerStats::Model::cmdErrorHander(const QString& err) { emit error(err); }

void ServerStats::Model::srvInfoCallback() {
  m_connection->cmd(
      {"INFO", "all"}, this, -1,
      [this](const RedisClient::Response& r) {
        m_serverInfo = RedisClient::ServerInfo::fromString(
                           QString::fromUtf8(r.value().toByteArray()))
                           .parsed.toVariantMap();
        emit serverInfoChanged();
      },
      [this](const QString& e) { cmdErrorHander(e); });
}

void ServerStats::Model::slowLogCallback() {
  m_connection->cmd(
      {"SLOWLOG", "GET", "15"}, this, -1,
      [this](const RedisClient::Response& r) {
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
}

void ServerStats::Model::clientsCallback() {
  m_connection->cmd(
      {"CLIENT", "LIST"}, this, -1,
      [this](const RedisClient::Response& r) {
        QVariant result = r.value();
        QStringList lines = result.toString().split("\n");

        QVariantList parsedClients;

        for (auto rawLine : lines) {
          QStringList lineParts = rawLine.split(" ");
          QVariantMap parsed;

          for (auto linePart : lineParts) {
            QStringList keyAndVal = linePart.split("=");

            if (keyAndVal.size() > 1) {
              parsed.insert(keyAndVal[0], keyAndVal[1]);
            } else if (linePart.size() > 0) {
              parsed.insert(keyAndVal[0], "");
            }
          }
          if (parsed.size() > 0)
            parsedClients.append(parsed);
        }

        m_clients = parsedClients;
        emit clientsChanged();
      },
      [this](const QString& e) { cmdErrorHander(e); });
}
