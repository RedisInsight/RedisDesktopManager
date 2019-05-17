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

  void run(QSharedPointer<RedisClient::Connection> targetConnection =
               QSharedPointer<RedisClient::Connection>(),
           int targetDbIndex = 0) override;

  QString getTypeName() const override;

  bool multiConnectionOperation() const override;

 protected:
  void removeKeys();

 private:
  QSharedPointer<AsyncFuture::Combinator> m_combinator;
  QStringList m_errors;
  QMutex m_errorsMutex;
  QMutex m_removedKeysMutex;
};
}  // namespace BulkOperations
