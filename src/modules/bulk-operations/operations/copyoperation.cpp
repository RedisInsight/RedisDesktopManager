#include "copyoperation.h"

#include <QtConcurrent>

#define RESTORE_BUFFER_LIMIT 100

BulkOperations::CopyOperation::CopyOperation(
    QSharedPointer<RedisClient::Connection> connection, int dbIndex,
    OperationCallback callback, QRegExp keyPattern)
    : BulkOperations::AbstractOperation(connection, dbIndex, callback,
                                        keyPattern) {
  m_errorMessagePrefix = QCoreApplication::translate("RDM", "Cannot copy key ");
}

void BulkOperations::CopyOperation::performOperation(
    QSharedPointer<RedisClient::Connection> targetConnection,
    int targetDbIndex) {
  m_progress = 0;
  m_dumpedKeys = 0;
  m_errors.clear();

  auto returnResults = [this]() {
    m_callback(m_keyPattern, m_progress, m_errors);
  };

  if (m_affectedKeys.size() == 0) {
    return returnResults();
  }

  QByteArray ttl =
      QString::number(m_metadata["ttl"].toLongLong() * 1000).toUtf8();
  QByteArray replace = m_metadata["replace"].toString().toUpper().toUtf8();

  auto processKeyDumps = [this, returnResults, ttl, replace, targetConnection,
                          targetDbIndex](const RedisClient::Response& r,
                                         QString err) {
    if (!err.isEmpty()) {
      return processError(err);
    }

    QMutexLocker l(&m_processedKeysMutex);
    QVariant incrResult = r.value();

    auto getRestoreCmd = [this, r, replace, ttl](const QByteArray& dump) {
      QList<QByteArray> restoreCmd{
          "RESTORE", m_affectedKeys[m_dumpedKeys].toUtf8(), ttl, dump};
      if (!replace.isEmpty()) {
        restoreCmd.append(replace);
      }
      return restoreCmd;
    };

    if (incrResult.canConvert(QVariant::ByteArray)) {
      m_restoreBuffer.append(getRestoreCmd(incrResult.toByteArray()));
      m_dumpedKeys++;
    } else if (incrResult.canConvert(QVariant::List)) {
      auto responses = incrResult.toList();

      for (auto resp : responses) {
        m_restoreBuffer.append(getRestoreCmd(resp.toByteArray()));
        m_dumpedKeys++;
      }
    }

    if (m_restoreBuffer.size() > RESTORE_BUFFER_LIMIT ||
        m_dumpedKeys == m_affectedKeys.size()) {
      targetConnection->pipelinedCmd(
          m_restoreBuffer, this, targetDbIndex,
          [this, returnResults](const RedisClient::Response& r, QString err) {
            if (!err.isEmpty()) {
              return processError(err);
            }
            QVariant incrResult = r.value();
            QMutexLocker l(&m_processedKeysMutex);

            if (incrResult.canConvert(QVariant::ByteArray)) {
              if (r.isErrorMessage()) {
                return processError(incrResult.toString());
              }
              m_progress++;
            } else if (incrResult.canConvert(QVariant::List)) {
              auto responses = incrResult.toList();

              for (auto resp : responses) {
                if (resp.toString().startsWith("ERR")) {
                  return processError(resp.toString());
                }

                m_progress++;
              }
            }

            emit progress(m_progress);

            if (m_progress >= m_affectedKeys.size()) {
              returnResults();
            }
          });
      m_restoreBuffer.clear();
    }
  };

  auto processKeys = [this, processKeyDumps]() {
    QList<QList<QByteArray>> rawCmds;

    for (QString k : m_affectedKeys) {
      rawCmds.append({"DUMP", k.toUtf8()});
    }

    m_connection->pipelinedCmd(rawCmds, this, -1, processKeyDumps);
  };

  auto verifySourceConnection = [this, processKeys, targetConnection]() {
    m_connection->cmd(
        {"ping"}, this, m_dbIndex,
        [processKeys, this](const RedisClient::Response& r) {
          if (r.isErrorMessage()) {
            return processError(
                QCoreApplication::translate("RDM", "Source connection error"));
          }
          QtConcurrent::run(processKeys);
        },
        [this](const QString& err) { processError(err); });
  };

  targetConnection->cmd(
      {"ping"}, this, targetDbIndex,
      [verifySourceConnection, this](const RedisClient::Response& r) {
        if (r.isErrorMessage()) {
          return processError(
              QCoreApplication::translate("RDM", "Target connection error"));
        }

        verifySourceConnection();
      },
      [this](const QString& err) { processError(err); });
}
