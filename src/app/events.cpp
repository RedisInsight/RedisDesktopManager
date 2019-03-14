#include "events.h"

void Events::registerLoggerForConnection(RedisClient::Connection& c) {
  QObject::connect(&c, &RedisClient::Connection::log, this,
                   [this](const QString& info) {
                     emit log(QString("Connection: %1").arg(info));
                   }, Qt::QueuedConnection);

  QObject::connect(&c, &RedisClient::Connection::error, this,
                   [this](const QString& error) {
                     emit log(QString("Connection: %1").arg(error));
                   }, Qt::QueuedConnection);
}
