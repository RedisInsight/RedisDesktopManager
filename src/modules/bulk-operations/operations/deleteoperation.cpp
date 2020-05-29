#include "deleteoperation.h"
#include <QtConcurrent>

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

        QtConcurrent::run(this, &DeleteOperation::deleteKeys,
                          m_affectedKeys, removalCmd, [this, removalCmd, returnResults](){
            // Retry on keys with errors
            if (m_keysWithErrors.size() > 0) {
                m_errors.clear();
                deleteKeys(QStringList(m_keysWithErrors), removalCmd, returnResults);
            } else {
                returnResults();
            }
        });
  });
}

void BulkOperations::DeleteOperation::deleteKeys(
        const QStringList &keys,
        const QByteArray &rmCmd, std::function<void()> callback)
{
    m_combinator->subscribe(callback, callback);

    for (QString k : keys) {
      auto future = m_connection->cmd(
          {rmCmd, k.toUtf8()}, this, -1,
          [this](const RedisClient::Response&) {
            QMutexLocker l(&m_processedKeysMutex);
            m_progress++;
            emit progress(m_progress);
          },
          [this, k](const QString& err) {
            QMutexLocker l(&m_errorsMutex);
            m_keysWithErrors.append(k);
            m_errors.append(
                QCoreApplication::translate("RDM", "Cannot remove key ") +
                QString("%1: %2").arg(k).arg(err));
          });

      m_combinator->combine(future);
    }
}
