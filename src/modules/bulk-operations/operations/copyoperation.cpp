#include "copyoperation.h"

BulkOperations::CopyOperation::CopyOperation(
    QSharedPointer<RedisClient::Connection> connection, int dbIndex,
    OperationCallback callback, QRegExp keyPattern)
    : BulkOperations::AbstractOperation(connection, dbIndex, callback,
                                        keyPattern) {}

void BulkOperations::CopyOperation::performOperation(
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

  auto errCallback = [this](const QString& k) {
    return [this, k](const QString& err) {
      QMutexLocker l(&m_errorsMutex);
      m_errors.append(QCoreApplication::translate("RDM", "Cannot copy key ") +
                      QString("%1: %2").arg(k).arg(err));
    };
  };

  QByteArray ttl =
      QString::number(m_metadata["ttl"].toLongLong() * 1000).toUtf8();
  QByteArray replace = m_metadata["replace"].toString().toUpper().toUtf8();

  for (QString k : m_affectedKeys) {
    auto err = errCallback(k);

    auto future = m_connection->cmd(
        {"DUMP", k.toUtf8()}, this, m_dbIndex,
        [=](const RedisClient::Response& r) {
          QList<QByteArray> cmd = {"RESTORE", k.toUtf8(), ttl,
                                   r.value().toByteArray()};

          if (!replace.isEmpty()) {
            cmd.append(replace);
          }

          auto targetFuture = targetConnection->cmd(
              cmd, this, targetDbIndex,
              [this](const RedisClient::Response&) {
                QMutexLocker l(&m_processedKeysMutex);
                m_progress++;
                emit progress(m_progress);
              },
              err);

          m_combinator->combine(targetFuture);
        },
        err);

    m_combinator->combine(future);
  }

  m_combinator->subscribe(returnResults, returnResults);
}
