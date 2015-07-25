#include "setkey.h"
#include "modules/redisclient/command.h"
#include "modules/redisclient/commandexecutor.h"

SetKeyModel::SetKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl)
       : ListLikeKeyModel(connection, fullPath, dbIndex, ttl,
                          "SCARD", "SSCAN %1 0 COUNT 10000", "SMEMBERS", false)
{    
}

QString SetKeyModel::getType()
{
    return "set";
}

void SetKeyModel::updateRow(int rowIndex, const QVariantMap &row)
{
    if (!isRowLoaded(rowIndex) || !isRowValid(row))
        throw Exception("Invalid row");

    QByteArray cachedRow = m_rowsCache[rowIndex];
    QByteArray newRow(row["value"].toByteArray());

    // delete old value
    deleteSetRow(cachedRow);
    m_rowsCache.removeAt(rowIndex);

    // add new value
    addSetRow(newRow);
    m_rowsCache.insert(rowIndex, newRow);
}

void SetKeyModel::addRow(const QVariantMap &row)
{
    if (!isRowValid(row))
        throw Exception("Invalid row");

    addSetRow(row["value"].toByteArray());
    m_rowCount++;
}

void SetKeyModel::removeRow(int i)
{
    if (!isRowLoaded(i))
        return;

    QByteArray value = m_rowsCache.value(i);   
    deleteSetRow(value);

    m_rowCount--;
    m_rowsCache.removeAt(i);

    setRemovedIfEmpty();
}

void SetKeyModel::addSetRow(const QByteArray &value)
{
    using namespace RedisClient;
    Command addCmd(m_dbIndex);
    (addCmd << "SADD" << m_keyFullPath).append(value);

    try {
        CommandExecutor::execute(m_connection, addCmd);
    } catch (const RedisClient::CommandExecutor::Exception& e) {
        throw Exception("Connection error: " + QString(e.what()));
    }
}

RedisClient::Response SetKeyModel::deleteSetRow(const QByteArray &value)
{
    using namespace RedisClient;
    Command deleteCmd(m_dbIndex);
    (deleteCmd << "SREM" << m_keyFullPath).append(value);

    Response result;

    try {
        result = CommandExecutor::execute(m_connection, deleteCmd);
    } catch (const RedisClient::CommandExecutor::Exception& e) {
        throw Exception("Connection error: " + QString(e.what()));
    }
    return result;
}
