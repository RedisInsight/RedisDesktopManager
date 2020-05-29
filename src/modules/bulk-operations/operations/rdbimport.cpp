#include "rdbimport.h"
#include <qpython.h>
#include <QFileInfo>
#include <QtConcurrent>

BulkOperations::RDBImportOperation::RDBImportOperation(
    QSharedPointer<RedisClient::Connection> connection, int dbIndex,
    OperationCallback callback, QSharedPointer<QPython> p, QRegExp keyPattern)
    : BulkOperations::AbstractOperation(connection, dbIndex, callback,
                                        keyPattern),
      m_python(p) {
  m_python->importModule_sync("rdb");
}

void BulkOperations::RDBImportOperation::getAffectedKeys(
    std::function<void(QVariant, QString)> callback) {
  m_python->call_native(
      "rdb.rdb_list_keys",
      QVariantList{m_metadata["path"].toString(), m_metadata["db"].toInt(),
                   m_keyPattern.pattern()},
      [callback, this](QVariant v) {
        m_affectedKeys.clear();

        QVariantList keys = v.toList();

        for (QVariant k : keys) {
          m_affectedKeys.append(QString::fromUtf8(k.toByteArray()));
        }

        return callback(QVariant(m_affectedKeys), "");
      });
}

bool BulkOperations::RDBImportOperation::isMetadataValid() const {
  return m_metadata.contains("db") && m_metadata.contains("path") &&
         QFileInfo::exists(m_metadata["path"].toString());
}

QList<QByteArray> convertToByteArray(QVariant v) {
  QVariantList l = v.toList();

  QList<QByteArray> result;

  for (QVariant b : l) {
    result.append(b.toByteArray());
  }

  return result;
}

void BulkOperations::RDBImportOperation::performOperation(
    QSharedPointer<RedisClient::Connection>, int) {
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

  auto processCommands = [this, returnResults](const QVariantList& commands) {
    m_combinator->subscribe(returnResults, returnResults);

    for (QVariant cmd : commands) {
      auto rawCmd = convertToByteArray(cmd);

      if (rawCmd.at(0).toLower() == QByteArray("select")) {
          continue;
      }

      auto future = m_connection->cmd(
          rawCmd, this, -1,
          [this](const RedisClient::Response&) { incrementProgress(); },
          [this, rawCmd](const QString& err) {
            QMutexLocker l(&m_errorsMutex);
            m_errors.append(
                QCoreApplication::translate("RDM", "Cannot execute command ") +
                QString("%1: %2")
                    .arg(QString::fromUtf8(rawCmd.join(QByteArray(" "))))
                    .arg(err));
          });

      m_combinator->combine(future);
    }
  };

  m_python->call_native(
      "rdb.rdb_export_as_commands",
      QVariantList{m_metadata["path"].toString(), m_metadata["db"].toInt(),
                   m_keyPattern.pattern()},
      [processCommands, this](QVariant v) {
        QVariantList commands = v.toList();

        m_connection->cmd(
                {"select", QByteArray::number(m_dbIndex)}, this, -1,
                [processCommands, commands](const RedisClient::Response&) {
            QtConcurrent::run(processCommands, commands);
        }, [](const QString&) {});
      });
}
