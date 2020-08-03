#pragma once
#include <asyncfuture.h>
#include <QObject>
#include <QRegExp>
#include <QSharedPointer>
#include "abstractoperation.h"

namespace BulkOperations {

class TtlOperation : public AbstractOperation {
  Q_OBJECT
 public:
  TtlOperation(QSharedPointer<RedisClient::Connection> connection, int dbIndex,
               OperationCallback callback,
               QRegExp keyPattern = QRegExp("*", Qt::CaseSensitive,
                                            QRegExp::Wildcard));

  QString getTypeName() const override { return QString("ttl"); }

  bool multiConnectionOperation() const override { return false; }

  bool isMetadataValid() const override { return m_metadata.contains("ttl"); }

 protected:
  void performOperation(
      QSharedPointer<RedisClient::Connection> targetConnection,
      int targetDbIndex) override;


  void setTtl(const QStringList& keys, const QByteArray &ttl, std::function<void()> callback);
};
}  // namespace BulkOperations
