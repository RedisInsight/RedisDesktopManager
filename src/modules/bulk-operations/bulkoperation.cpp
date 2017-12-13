#include "bulkoperation.h"
#include <qredisclient/connection.h>

BulkOperations::CurrentOperation::CurrentOperation(QSharedPointer<RedisClient::Connection> connection,
                                                   int dbIndex, BulkOperations::Manager::Operation op,
                                                   QRegExp keyPattern)
    : m_connection(connection),
      m_dbIndex(dbIndex),
      m_type(op),
      m_keyPattern(keyPattern),
      m_currentState(State::READY),
      m_progress(0)
{
}

void BulkOperations::CurrentOperation::getAffectedKeys(std::function<void (QVariant, QString)> callback)
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

void BulkOperations::CurrentOperation::run(std::function<void (RedisClient::Response, QString)> callback,
                                           QSharedPointer<RedisClient::Connection>,int)
{
    QList<QByteArray> rawCmd {"DEL"};

    for (QString k: m_affectedKeys) {
        rawCmd.append(k.toUtf8());
    }

    try {
        m_connection->command(rawCmd, this, callback);
    } catch (const RedisClient::Connection::Exception& e) {
        callback(RedisClient::Response(), QString(e.what()));
    }
}

QString BulkOperations::CurrentOperation::getTypeName() const
{
    switch (m_type) {
        case Manager::Operation::DELETE_KEYS: return QString("delete_keys");
        case Manager::Operation::COPY_KEYS: return QString("copy_keys");
        default: return QString();
    }
}

bool BulkOperations::CurrentOperation::multiConnectionOperation() const
{
    return m_type == Manager::Operation::COPY_KEYS;
}
