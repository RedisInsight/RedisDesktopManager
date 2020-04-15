#pragma once
#include <asyncfuture.h>
#include <QObject>
#include <QRegExp>
#include <QSharedPointer>
#include "abstractoperation.h"

namespace BulkOperations {

class DeleteOperation : public AbstractOperation {
  Q_OBJECT
 public:
  DeleteOperation(QSharedPointer<RedisClient::Connection> connection,
                  int dbIndex, OperationCallback callback,
                  QRegExp keyPattern = QRegExp("*", Qt::CaseSensitive,
                                               QRegExp::Wildcard));

  QString getTypeName() const override { return QString("delete_keys"); }

  bool multiConnectionOperation() const override { return false; }

 protected:
  void performOperation(
      QSharedPointer<RedisClient::Connection> targetConnection,
      int targetDbIndex) override;

  void deleteKeys(const QStringList& keys,
                  const QByteArray& rmCmd,
                  std::function<void()> callback);
};
}  // namespace BulkOperations
