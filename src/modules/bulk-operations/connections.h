#pragma once
#include <QSharedPointer>
#include <QStringList>

namespace RedisClient {
class Connection;
}

namespace BulkOperations {

class ConnectionsModel {
 public:
  virtual QSharedPointer<RedisClient::Connection> getByIndex(int index) = 0;

  virtual QStringList getConnections() = 0;
};
}  // namespace BulkOperations
