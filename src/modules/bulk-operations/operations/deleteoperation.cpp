#include "deleteoperation.h"

BulkOperations::DeleteOperation::DeleteOperation(
    QSharedPointer<RedisClient::Connection> connection, int dbIndex,
    OperationCallback callback, QRegExp keyPattern)
    : BulkOperations::AbstractOperation(connection, dbIndex, callback,
                                        keyPattern) {}

void BulkOperations::DeleteOperation::performOperation(
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

  AsyncFuture::observe(m_connection->isCommandSupported({"UNLINK"}))
      .subscribe([this, returnResults](bool supportUnlink) {
        QByteArray removalCmd{"DEL"};

        if (supportUnlink) {
          removalCmd = "UNLINK";
        }

        for (QString k : m_affectedKeys) {
          auto future = m_connection->cmd(
              {removalCmd, k.toUtf8()}, this, m_dbIndex,
              [this](const RedisClient::Response&) {
                QMutexLocker l(&m_processedKeysMutex);
                m_progress++;
                emit progress(m_progress);
              },
              [this, k](const QString& err) {
                QMutexLocker l(&m_errorsMutex);
                m_errors.append(
                    QCoreApplication::translate("RDM", "Cannot remove key ") +
                    QString("%1: %2").arg(k).arg(err));
              });

          m_combinator->combine(future);
        }

        m_combinator->subscribe(returnResults, returnResults);
      });
}
