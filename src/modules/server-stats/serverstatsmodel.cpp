#include "serverstatsmodel.h"
#include <QCoreApplication>

ServerStats::Model::Model(QSharedPointer<RedisClient::Connection> connection,
                          int dbIndex)
    : TabModel(connection, dbIndex) {
  m_serverInfoUpdateTimer.setInterval(5000);
  m_serverInfoUpdateTimer.setSingleShot(false);
  m_slowLogUpdateTimer.setInterval(5000);
  m_slowLogUpdateTimer.setSingleShot(false);
  m_clientsUpdateTimer.setInterval(5000);
  m_clientsUpdateTimer.setSingleShot(false);

  QObject::connect(&m_serverInfoUpdateTimer, &QTimer::timeout, this, [this] {
    m_connection->command(
        {"INFO", "all"}, this, [this](RedisClient::Response r, QString err) {
          if (!err.isEmpty()) {
            emit error(QCoreApplication::translate(
                           "RDM", "Cannot update server info tab. Error: %0")
                           .arg(err));
            return;
          }

          m_serverInfo = RedisClient::ServerInfo::fromString(r.toRawString(-1))
                             .parsed.toVariantMap();
          emit serverInfoChanged();
        });
  });

  QObject::connect(&m_slowLogUpdateTimer, &QTimer::timeout, this, [this] {
    m_connection->command(
        {"SLOWLOG", "GET", "15"}, this,
        [this](RedisClient::Response r, QString err) {
          if (!err.isEmpty()) {
            emit error(QCoreApplication::translate(
                           "RDM", "Cannot update slowlog. Error: %0")
                           .arg(err));
            return;
          }

          QVariantList processed;

          for (QVariant item : r.getValue().toList()) {
            auto itemList = item.toList();
            QVariantMap row;
            row.insert("time", itemList[1]);
            row.insert("exec_time", itemList[2]);
            row.insert("cmd", itemList[3]);
            processed.append(row);
          }

          m_slowLog = processed;
          emit slowLogChanged();
        });
  });

  QObject::connect(&m_clientsUpdateTimer, &QTimer::timeout, this, [this] {
    m_connection->command(
        {"CLIENT", "LIST"}, this, [this](RedisClient::Response r, QString err) {
          if (!err.isEmpty()) {
            emit error(QCoreApplication::translate(
                           "RDM", "Cannot update clients list. Error: %0")
                           .arg(err));
            return;
          }

          QVariant result = r.getValue();
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
        });
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
