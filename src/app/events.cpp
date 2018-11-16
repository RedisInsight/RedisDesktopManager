#include "events.h"

void Events::registerLoggerForConnection(RedisClient::Connection& c) {
  QObject::connect(&c, &RedisClient::Connection::log,
                   [this](const QString& info) {
                     emit log(QString("Connection: %1").arg(info));
                   });

  QObject::connect(&c, &RedisClient::Connection::error,
                   [this](const QString& error) {
                     emit log(QString("Connection: %1").arg(error));
                   });
}
