#include "copyoperation.h"
#include <QtConcurrent>

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

  auto processKeys =
      [this, errCallback, returnResults, ttl,
       replace](QSharedPointer<RedisClient::Connection> targetConnection) {

        m_combinator->subscribe(returnResults, returnResults);

        for (QString k : m_affectedKeys) {
          auto err = errCallback(k);

          auto restoreKeyOnTargetServer = [=](const RedisClient::Response& r) {
            QList<QByteArray> cmd = {"RESTORE", k.toUtf8(), ttl,
                                     r.value().toByteArray()};

            if (!replace.isEmpty()) {
              cmd.append(replace);
            }

            auto targetFuture = targetConnection->cmd(
                cmd, this, -1,
                [this](const RedisClient::Response& r) {
                  QMutexLocker l(&m_processedKeysMutex);
                  m_progress++;
                  emit progress(m_progress);
                },
                err);

            m_combinator->combine(targetFuture);
          };

          auto future = m_connection->cmd({"DUMP", k.toUtf8()}, this, -1,
                                          restoreKeyOnTargetServer, err);

          m_combinator->combine(future);
        }
      };

  auto verifySourceConnection = [this, processKeys, targetConnection,
                                 returnResults]() {
    m_connection->cmd(
        {"select", QByteArray::number(m_dbIndex)}, this, -1,
        [processKeys, targetConnection](const RedisClient::Response&) {
          QtConcurrent::run(processKeys, targetConnection);
        },
        [this, returnResults](const QString& err) {
          QMutexLocker l(&m_errorsMutex);
          m_errors.append(
              QCoreApplication::translate(
                  "RDM", "Cannot connect to source redis-server: %1")
                  .arg(err));
          returnResults();
        });
  };

  targetConnection->cmd(
      {"select", QByteArray::number(targetDbIndex)}, this, -1,
      [verifySourceConnection](const RedisClient::Response&) {
        verifySourceConnection();
      },
      [this, returnResults](const QString& err) {
        QMutexLocker l(&m_errorsMutex);
        m_errors.append(QCoreApplication::translate(
                            "RDM", "Cannot connect to target redis-server: %1")
                            .arg(err));
        returnResults();
      });
}
