#include "ttloperation.h"

BulkOperations::TtlOperation::TtlOperation(
    QSharedPointer<RedisClient::Connection> connection, int dbIndex,
    OperationCallback callback, QRegExp keyPattern)
    : BulkOperations::AbstractOperation(connection, dbIndex, callback,
                                        keyPattern) {}

void BulkOperations::TtlOperation::performOperation(
    QSharedPointer<RedisClient::Connection> targetConnection,
    int targetDbIndex) {
  m_combinator = QSharedPointer<AsyncFuture::Combinator>(
      new AsyncFuture::Combinator(AsyncFuture::FailFast));

  m_progress = 0;
  m_errors.clear();

  auto returnResults = [this]() {
    m_callback(m_keyPattern, m_progress, m_errors);
  };

  if (m_affectedKeys.size() == 0) {
    return returnResults();
  }

  QByteArray ttl = m_metadata["ttl"].toString().toUtf8();

  for (QString k : m_affectedKeys) {
    auto future = m_connection->cmd(
        {"EXPIRE", k.toUtf8(), ttl}, this, m_dbIndex,
        [this](const RedisClient::Response&) {
          QMutexLocker l(&m_processedKeysMutex);
          m_progress++;
          emit progress(m_progress);
        },
        [this, k](const QString& err) {
          QMutexLocker l(&m_errorsMutex);
          m_errors.append(
              QCoreApplication::translate("RDM", "Cannot set TTL for key ") +
              QString("%1: %2").arg(k).arg(err));
        });

    m_combinator->combine(future);
  }

  m_combinator->subscribe(returnResults, returnResults);
}
