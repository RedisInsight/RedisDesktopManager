#include "deleteoperation.h"

#include <QtConcurrent>

BulkOperations::DeleteOperation::DeleteOperation(
    QSharedPointer<RedisClient::Connection> connection, int dbIndex,
    OperationCallback callback, QRegExp keyPattern)
    : BulkOperations::AbstractOperation(connection, dbIndex, callback,
                                        keyPattern) {
  m_errorMessagePrefix =
      QCoreApplication::translate("RESP", "Cannot remove key ");
}

void BulkOperations::DeleteOperation::performOperation(
    QSharedPointer<RedisClient::Connection>, int) {
  m_progress = 0;
  m_errors.clear();

  auto returnResults = [this]() {
    qDebug() << "Processed keys: " << m_progress;
    m_callback(m_keyPattern, m_progress, m_errors);
  };

  if (m_affectedKeys.size() == 0) {
    return returnResults();
  }

  AsyncFuture::observe(m_connection->isCommandSupported({"UNLINK"}))
      .subscribe([this, returnResults](bool supportUnlink) {
        QByteArray removalCmd{"DEL"};

        if (supportUnlink) {
          removalCmd = "UNLINK";
        }

        QtConcurrent::run(this, &DeleteOperation::deleteKeys, m_affectedKeys,
                          removalCmd, [this, removalCmd, returnResults]() {
                            // Retry on keys with errors
                            if (m_keysWithErrors.size() > 0) {
                              m_errors.clear();
                              deleteKeys(m_keysWithErrors,
                                         removalCmd, returnResults);
                            } else {
                              returnResults();
                            }
                          });
      });
}

void BulkOperations::DeleteOperation::deleteKeys(
    const QList<QByteArray> &keys, const QByteArray &rmCmd,
    std::function<void()> callback) {
  QList<QList<QByteArray>> rawCmds;

  for (const QByteArray& k : keys) {
    rawCmds.append({rmCmd, k});
  }

  int batchSize = m_connection->pipelineCommandsLimit();
  int expectedResponses = rawCmds.size();

  m_connection->pipelinedCmd(
      rawCmds, this, m_dbIndex,
      [this, expectedResponses, callback, batchSize](
          const RedisClient::Response &r, QString err) {
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
