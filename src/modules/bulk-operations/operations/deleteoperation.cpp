#include "deleteoperation.h"

#include <QtConcurrent>

BulkOperations::DeleteOperation::DeleteOperation(
    QSharedPointer<RedisClient::Connection> connection, int dbIndex,
    OperationCallback callback, QRegExp keyPattern)
    : BulkOperations::AbstractOperation(connection, dbIndex, callback,
                                        keyPattern) {
  m_errorMessagePrefix =
      QCoreApplication::translate("RDM", "Cannot remove key ");
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
                              deleteKeys(QStringList(m_keysWithErrors),
                                         removalCmd, returnResults);
                            } else {
                              returnResults();
                            }
                          });
      });
}

void BulkOperations::DeleteOperation::deleteKeys(
    const QStringList &keys, const QByteArray &rmCmd,
    std::function<void()> callback) {
  QList<QList<QByteArray>> rawCmds;

  for (QString k : keys) {
    rawCmds.append({rmCmd, k.toUtf8()});
  }

  int expectedResponses = rawCmds.size();

  m_connection->pipelinedCmd(
      rawCmds, this, m_dbIndex,
      [this, expectedResponses, callback](const RedisClient::Response &r,
                                          QString err) {
        if (!err.isEmpty()) {
          return processError(err);
        }

        if (r.isErrorMessage()) {
          return processError(r.value().toByteArray());
        }

        QMutexLocker l(&m_processedKeysMutex);
        QVariant incrResult = r.value();

        if (incrResult.canConvert(QVariant::ByteArray)) {
          m_progress++;
        } else if (incrResult.canConvert(QVariant::List)) {
          auto responses = incrResult.toList();

          for (auto resp : responses) {
            m_progress++;
          }
        }

        emit progress(m_progress);

        if (m_progress >= expectedResponses) {
          callback();
        }
      });
}
