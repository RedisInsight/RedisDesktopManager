#include "rdbimport.h"

#include <qpython.h>
#include <qredisclient/utils/text.h>

#include <QFileInfo>
#include <QtConcurrent>

BulkOperations::RDBImportOperation::RDBImportOperation(
    QSharedPointer<RedisClient::Connection> connection, int dbIndex,
    OperationCallback callback, QSharedPointer<QPython> p, QRegExp keyPattern)
    : BulkOperations::AbstractOperation(connection, dbIndex, callback,
                                        keyPattern),
      m_python(p) {
  m_python->importModule_sync("rdb");
  m_errorMessagePrefix =
      QCoreApplication::translate("RESP", "Cannot execute command ");
}

void BulkOperations::RDBImportOperation::getAffectedKeys(
    std::function<void(QVariant, QString)> callback) {

  m_keyPattern.setPatternSyntax(QRegExp::RegExp2);

  if (!m_keyPattern.isValid()) {
    return callback(QVariant(), QCoreApplication::translate(
                                    "RESP", "Invalid regexp for keys filter."));
  }

  m_python->call_native(
      "rdb.rdb_list_keys",
      QVariantList{m_metadata["path"].toString(), m_metadata["db"].toInt(),
                   m_keyPattern.pattern()},
      [callback, this](QVariant v) {
        m_affectedKeys.clear();

        if (v.isNull()) {
          return callback(QVariant(),
                          QCoreApplication::translate(
                              "RESP", "Cannot get the list of affected keys"));
        }

        QVariantList keys = v.toList();
        QStringList keyNames;

        for (const QVariant &k : qAsConst(keys)) {
          m_affectedKeys.append(k.toByteArray());
          keyNames.append(printableString(k.toByteArray(), true));
        }

        return callback(QVariant(keyNames), "");
      });
}

bool BulkOperations::RDBImportOperation::isMetadataValid() const {
  return m_metadata.contains("db") && m_metadata.contains("path") &&
         QFileInfo::exists(m_metadata["path"].toString());
}

QList<QByteArray> convertToByteArray(QVariant v) {
  QVariantList l = v.toList();

  QList<QByteArray> result;

  for (const QVariant &b : qAsConst(l)) {
    result.append(b.toByteArray());
  }

  return result;
}

void BulkOperations::RDBImportOperation::performOperation(
    QSharedPointer<RedisClient::Connection>, int) {
  m_progress = 0;
  m_errors.clear();

  auto returnResults = [this]() {
    m_callback(m_keyPattern, m_progress, m_errors);
  };

  if (m_affectedKeys.size() == 0) {
    return returnResults();
  }

  auto processCommands = [this, returnResults](const QVariantList& commands) {
    QList<QList<QByteArray>> rawCmds;

    for (const QVariant &cmd : commands) {
      auto rawCmd = convertToByteArray(cmd);

      if (rawCmd.at(0).toLower() == QByteArray("select")) {
        continue;
      }

      rawCmds.append(rawCmd);
    }

    int batchSize = m_connection->pipelineCommandsLimit();
    int expectedResponses = rawCmds.size();

    m_connection->pipelinedCmd(
        rawCmds, this, m_dbIndex,
        [this, returnResults, expectedResponses, batchSize](const RedisClient::Response& r,
                                                 const QString& err) {
          if (!err.isEmpty() || r.isErrorMessage()) {
            return processError(err.isEmpty()? r.value().toByteArray() : err);
          }

          {
            QMutexLocker l(&m_processedKeysMutex);            
            m_progress += batchSize;
            emit progress(m_progress);
          }

          if (m_progress >= expectedResponses) {
            returnResults();
          }
        }, false);
  };

  m_python->call_native(
      "rdb.rdb_export_as_commands",
      QVariantList{m_metadata["path"].toString(), m_metadata["db"].toInt(),
                   m_keyPattern.pattern()},
      [processCommands, this](QVariant v) {
        QVariantList commands = v.toList();

        m_connection->cmd(
            {"ping"}, this, m_dbIndex,
            [processCommands, commands, this](const RedisClient::Response& r) {
              if (r.isErrorMessage()) {
                return processError(QCoreApplication::translate(
                    "RESP", "Target connection error"));
              }
              QtConcurrent::run(processCommands, commands);
            },
            [this](const QString& err) { processError(err); });
      });
}
