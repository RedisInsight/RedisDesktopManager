#include "events.h"

void Events::registerLoggerForConnection(RedisClient::Connection& c) {
  auto self = sharedFromThis().toWeakRef();
  QObject::connect(
      &c, &RedisClient::Connection::log, this, [self](const QString& info) {
        if (!self) return;
        emit self.toStrongRef()->log(QString("Connection: %1").arg(info));
      }, Qt::QueuedConnection);

  QObject::connect(
      &c, &RedisClient::Connection::error, this, [self](const QString& error) {
        if (!self) return;
        emit self.toStrongRef()->log(QString("Connection: %1").arg(error));
      }, Qt::QueuedConnection);
}
