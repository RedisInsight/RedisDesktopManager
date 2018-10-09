#include "tabmodel.h"
#include <QtConcurrent>

TabModel::TabModel(QSharedPointer<RedisClient::Connection> connection,
                   int dbIndex)
    : m_dbIndex(dbIndex) {
  // Clone connection
  RedisClient::ConnectionConfig config = connection->getConfig();
  m_connection = QSharedPointer<RedisClient::Connection>(
      new RedisClient::Connection(config));
}

TabModel::~TabModel() {
  QtConcurrent::run(
      [](QSharedPointer<RedisClient::Connection> connection) {        
        connection->disconnect();
      },
      m_connection);

  m_connection.clear();
}

void TabModel::init() {
  auto weekPointer = sharedFromThis().toWeakRef();

  QObject::connect(m_connection.data(), &RedisClient::Connection::connected,
                   [this, weekPointer]() {
                     if (!weekPointer) {
                       return;
                     }

                     if (m_dbIndex) {
                       m_connection->command({"PING"}, m_dbIndex);
                     }

                     emit initialized();
                   });

  try {
    m_connection->connect(false);
  } catch (RedisClient::Connection::Exception&) {
    emit error(QObject::tr("Invalid Connection. Check connection settings."));
    return;
  }
}

QSharedPointer<RedisClient::Connection> TabModel::getConnection() const {
  return m_connection;
}
