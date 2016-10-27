#include "setkey.h"
#include <qredisclient/connection.h>

SetKeyModel::SetKeyModel(QSharedPointer<RedisClient::Connection> connection,
                         QByteArray fullPath, int dbIndex, long long ttl)
       : ListLikeKeyModel(connection, fullPath, dbIndex, ttl,
                          "SCARD", "SSCAN %1 0 COUNT 10000", QByteArray(), false)
{    
}

QString SetKeyModel::getType()
{
    return "set";
}

void SetKeyModel::updateRow(int rowIndex, const QVariantMap &row)
{
    if (!isRowLoaded(rowIndex) || !isRowValid(row))
        throw Exception(QObject::tr("Invalid row"));

    QByteArray cachedRow = m_rowsCache[rowIndex];
    QByteArray newRow(row["value"].toByteArray());

    deleteSetRow(cachedRow);
    addSetRow(newRow);
    m_rowsCache.replace(rowIndex, newRow);
}

void SetKeyModel::addRow(const QVariantMap &row)
{
    if (!isRowValid(row))
        throw Exception(QObject::tr("Invalid row"));

    addSetRow(row["value"].toByteArray());
    m_rowCount++;
}

void SetKeyModel::removeRow(int i)
{
    if (!isRowLoaded(i))
        return;

    QByteArray value = m_rowsCache[i];
    deleteSetRow(value);

    m_rowCount--;
    m_rowsCache.removeAt(i);

    setRemovedIfEmpty();
}

void SetKeyModel::addSetRow(const QByteArray &value)
{
    try {
        m_connection->commandSync({"SADD", m_keyFullPath, value}, m_dbIndex);
    } catch (const RedisClient::Connection::Exception& e) {
        throw Exception(QObject::tr("Connection error: ") + QString(e.what()));
    }
}

RedisClient::Response SetKeyModel::deleteSetRow(const QByteArray &value)
{
    try {
        return m_connection->commandSync({"SREM", m_keyFullPath, value}, m_dbIndex);
    } catch (const RedisClient::Connection::Exception& e) {
        throw Exception(QObject::tr("Connection error: ") + QString(e.what()));
    }
}
