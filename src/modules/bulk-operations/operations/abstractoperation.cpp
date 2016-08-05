#include "abstractoperation.h"

BulkOperations::AbstractOperation::AbstractOperation(
        QSharedPointer<RedisClient::Connection> connection,
        int dbIndex, QRegExp keyPattern)
    : m_connection(connection),
      m_dbIndex(dbIndex),      
      m_keyPattern(keyPattern),
      m_currentState(State::READY),
      m_progress(0)
{
}

void BulkOperations::AbstractOperation::getAffectedKeys(std::function<void (QVariant, QString)> callback)
{
    try {
        m_connection->getDatabaseKeys([this, callback](const RedisClient::Connection::RawKeysList& keys, const QString& err) {
            if (!err.isEmpty()) {
                return callback(QVariant(), err);
            }

            m_affectedKeys.clear();

            for (QByteArray k: keys) {
                m_affectedKeys.append(QString::fromUtf8(k));
            }

            return callback(QVariant(m_affectedKeys), "");
        }, m_keyPattern.pattern(), m_dbIndex);
    } catch (const RedisClient::Connection::Exception& e) {
        return callback(QVariant(), QString(e.what()));
    }
}

bool BulkOperations::AbstractOperation::isRunning() const
{
    return m_currentState == State::RUNNING;
}

QSharedPointer<RedisClient::Connection> BulkOperations::AbstractOperation::getConnection()
{
    return m_connection;
}

int BulkOperations::AbstractOperation::getDbIndex() const
{
    return m_dbIndex;
}

QRegExp BulkOperations::AbstractOperation::getKeyPattern() const
{
    return m_keyPattern;
}

unsigned short BulkOperations::AbstractOperation::currentProgress() const
{
    return m_progress;
}

void BulkOperations::AbstractOperation::setMetadata(const QVariantMap &meta)
{
    m_metadata = meta;
}
