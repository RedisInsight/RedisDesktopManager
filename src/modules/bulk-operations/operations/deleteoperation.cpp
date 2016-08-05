#include "deleteoperation.h"


BulkOperations::DeleteOperation::DeleteOperation(
        QSharedPointer<RedisClient::Connection> connection,
        int dbIndex, QRegExp keyPattern)
    : BulkOperations::AbstractOperation(connection, dbIndex, keyPattern)
{

}

void BulkOperations::DeleteOperation::run(OperationCallback callback,
                                          QSharedPointer<RedisClient::Connection>, int)
{
    QList<QByteArray> rawCmd {"DEL"};

    for (QString k: m_affectedKeys) {
        rawCmd.append(k.toUtf8());
    }

    try {
        m_connection->command(rawCmd, this, [this, callback](RedisClient::Response r, const QString& e) {
            QStringList errors;

            if (!e.isEmpty() || r.isErrorMessage()) {
                errors << QString(e.isEmpty()? r.getValue().toString() : e);
                callback(0, errors);
                return;
            }

            callback(m_affectedKeys.size(), errors);
        });
    } catch (const RedisClient::Connection::Exception& e) {
        QStringList errors;
        errors << QString(e.what());
        callback(0, errors);
    }
}

QString BulkOperations::DeleteOperation::getTypeName() const
{    
    return QString("delete_keys");
}

bool BulkOperations::DeleteOperation::multiConnectionOperation() const
{
    return false;
}
