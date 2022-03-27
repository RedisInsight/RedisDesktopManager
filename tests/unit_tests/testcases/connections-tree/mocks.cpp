#include "mocks.h"

#include <qredisclient/connection.h>

#include <QFuture>

Mock<ConnectionsTree::Operations> getOperations() {
  Mock<ConnectionsTree::Operations> operations;
  When(Method(operations, getNamespaceSeparator)).AlwaysReturn(":");
  When(Method(operations, defaultFilter)).AlwaysReturn("*");
  When(Method(operations, mode)).AlwaysReturn("default");
  When(Method(operations, disconnect)).Return();
  When(Method(operations, notifyDbWasUnloaded)).Return();
  return operations;
}

Mock<ConnectionsTree::Operations> getOperationsWithGetDatabases(
    RedisClient::DatabaseList db, const QString &err) {
  auto op = getOperations();

  When(Method(op, getDatabases))
      .AlwaysDo(
          [db, err](
              QSharedPointer<ConnectionsTree::Operations::GetDatabasesCallback>
                  cb) -> QFuture<void> {
            cb->rawCallback()(db, err);
            return QFuture<void>();
          });

  return op;
}

Mock<ConnectionsTree::Operations> getOperationsWithDbAndKeys(
    RedisClient::DatabaseList db, const QString &err, QList<QByteArray> keys) {
  auto op = getOperationsWithGetDatabases(db, err);

  When(Method(op, loadNamespaceItems))
      .Do([keys, err](
              uint, const QString &,
              QSharedPointer<
                  ConnectionsTree::Operations::LoadNamespaceItemsCallback>
                  cb) -> void { cb->rawCallback()(keys, err); });

  return op;
}
