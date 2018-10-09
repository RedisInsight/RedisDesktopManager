#include "serverstatsmodel.h"

ServerStats::Model::Model(QSharedPointer<RedisClient::Connection> connection,
                          int dbIndex)
    : TabModel(connection, dbIndex) {
  m_updateTimer.setInterval(5000);
  m_updateTimer.setSingleShot(false);

  auto weekPtr = sharedFromThis().toWeakRef();

  QObject::connect(&m_updateTimer, &QTimer::timeout, this, [this, weekPtr] {
    if (!weekPtr) return;

    QList<QByteArray> rawCmd{"INFO", "all"};
    m_connection->command(
        rawCmd, this, [this](RedisClient::Response r, QString err) {
          if (!err.isEmpty()) {
            emit error(QObject::tr("Cannot update server info tab. Error: %0")
                           .arg(err));
            return;
          }

          m_serverInfo = RedisClient::ServerInfo::fromString(r.toRawString())
                             .parsed.toVariantMap();
          emit serverInfoChanged();
        });
  });

  QObject::connect(this, &TabModel::initialized,
                   [this]() { m_updateTimer.start(); });
}

QString ServerStats::Model::getName() const {
  return QString(QObject::tr("Server %0"))
      .arg(m_connection->getConfig().name());
}

QVariantMap ServerStats::Model::serverInfo() { return m_serverInfo; }
