#include "ttloperation.h"

#include <QtConcurrent>

BulkOperations::TtlOperation::TtlOperation(
    QSharedPointer<RedisClient::Connection> connection, int dbIndex,
    OperationCallback callback, QRegExp keyPattern)
    : BulkOperations::AbstractOperation(connection, dbIndex, callback,
                                        keyPattern) {
  m_errorMessagePrefix =
      QCoreApplication::translate("RESP", "Cannot set TTL for key ");
}

void BulkOperations::TtlOperation::performOperation(
    QSharedPointer<RedisClient::Connection>, int) {
  m_progress = 0;
  m_errors.clear();

  auto returnResults = [this]() {
    m_callback(m_keyPattern, m_progress, m_errors);
  };

  if (m_affectedKeys.size() == 0) {
    return returnResults();
  }

  QByteArray ttl = m_metadata["ttl"].toString().toUtf8();

  QtConcurrent::run(this, &TtlOperation::setTtl, m_affectedKeys, ttl,
                    [this, ttl, returnResults]() {
                      // Retry on keys with errors
                      if (m_keysWithErrors.size() > 0) {
                        m_errors.clear();
                        setTtl(m_keysWithErrors, ttl,
                               returnResults);
                      } else {
                        returnResults();
                      }
                    });
}

void BulkOperations::TtlOperation::setTtl(const QList<QByteArray>& keys,
                                          const QByteArray& ttl,
                                          std::function<void()> callback) {
  QList<QList<QByteArray>> rawCmds;

  for (const QByteArray& k : keys) {
    rawCmds.append({"EXPIRE", k, ttl});
  }

  int batchSize = m_connection->pipelineCommandsLimit();
  int expectedResponses = rawCmds.size();

  m_connection->pipelinedCmd(
      rawCmds, this, -1,
      [this, expectedResponses, callback, batchSize](
          const RedisClient::Response& r, QString err) {
        if (!err.isEmpty() || r.isErrorMessage()) {
          return processError(err.isEmpty() ? r.value().toByteArray() : err);
        }

        {
          QMutexLocker l(&m_processedKeysMutex);
          m_progress += batchSize;
          emit progress(m_progress);
        }

        if (m_progress >= expectedResponses) {
          callback();
        }
      },
      false);
}
