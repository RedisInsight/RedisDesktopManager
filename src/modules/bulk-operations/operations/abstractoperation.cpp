#include "abstractoperation.h"

BulkOperations::AbstractOperation::AbstractOperation(
    QSharedPointer<RedisClient::Connection> connection, int dbIndex,
    OperationCallback callback, QRegExp keyPattern)
    : m_connection(connection),
      m_dbIndex(dbIndex),
      m_keyPattern(keyPattern),
      m_currentState(State::READY),
      m_progress(0),
      m_callback(callback),
      m_lastProgressNotification(0) {}

void BulkOperations::AbstractOperation::getAffectedKeys(
    std::function<void(QVariant, QString)> callback) {
  auto processingCallback =
      [this, callback](const RedisClient::Connection::RawKeysList& keys,
                       const QString& err) {
        if (!err.isEmpty()) {
          return callback(QVariant(), err);
        }

        m_affectedKeys.clear();

        for (QByteArray k : keys) {
          m_affectedKeys.append(QString::fromUtf8(k));
        }

        return callback(QVariant(m_affectedKeys), "");
      };

  try {
    if (!m_connection->connect(true)) {
      return callback(QVariant(), QCoreApplication::translate(
                                      "RDM", "Cannot connect to redis-server"));
    }

    if (m_connection->mode() == RedisClient::Connection::Mode::Cluster) {
      m_connection->getClusterKeys(processingCallback, m_keyPattern.pattern());
    } else {
      m_connection->getDatabaseKeys(processingCallback, m_keyPattern.pattern(),
                                    m_dbIndex);
    }

  } catch (const RedisClient::Connection::Exception& e) {
    return callback(QVariant(), QString(e.what()));
  }
}

void BulkOperations::AbstractOperation::run(
    QSharedPointer<RedisClient::Connection> targetConnection,
    int targetDbIndex) {
  if (!isMetadataValid()) {
    qWarning() << QString("Invalid metadata for %1").arg(getTypeName());
    return;
  }

  if (m_affectedKeys.size() > 0) {
    performOperation(targetConnection, targetDbIndex);
  } else {
    getAffectedKeys([this, targetConnection, targetDbIndex](QVariant, QString) {
      performOperation(targetConnection, targetDbIndex);
    });
  }
}

bool BulkOperations::AbstractOperation::isRunning() const {
  return m_currentState == State::RUNNING;
}

QSharedPointer<RedisClient::Connection>
BulkOperations::AbstractOperation::getConnection() {
  return m_connection;
}

int BulkOperations::AbstractOperation::getDbIndex() const { return m_dbIndex; }

QRegExp BulkOperations::AbstractOperation::getKeyPattern() const {
  return m_keyPattern;
}

void BulkOperations::AbstractOperation::setKeyPattern(const QRegExp p) {
  m_keyPattern = p;
}

int BulkOperations::AbstractOperation::currentProgress() const {
  return m_progress;
}

void BulkOperations::AbstractOperation::setMetadata(const QVariantMap& meta) {
  m_metadata = meta;
}

void BulkOperations::AbstractOperation::incrementProgress() {
  QMutexLocker l(&m_processedKeysMutex);
  m_progress++;

  if (QDateTime::currentMSecsSinceEpoch() - m_lastProgressNotification >=
      1000) {
    qDebug() << "Notify UI about progress";
    emit progress(m_progress);
    m_lastProgressNotification = QDateTime::currentMSecsSinceEpoch();
  }
}

void BulkOperations::AbstractOperation::processError(const QString& err) {
  QMutexLocker l(&m_errorsMutex);
  m_errors.append(m_errorMessagePrefix + err);
  m_callback(m_keyPattern, m_progress, m_errors);
}
