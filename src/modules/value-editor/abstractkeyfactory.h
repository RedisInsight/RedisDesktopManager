#pragma once

#include <QSharedPointer>
#include <QString>
#include <QVariantMap>
#include <functional>
#include "keymodel.h"

namespace RedisClient {
class Connection;
}

namespace ValueEditor {

class AbstractKeyFactory {
 public:
  virtual ~AbstractKeyFactory() {}

  virtual void loadKey(
      QSharedPointer<RedisClient::Connection> connection,
      QByteArray keyFullPath, int dbIndex,
      std::function<void(QSharedPointer<Model>, const QString&)> callback) = 0;
};

}  // namespace ValueEditor
