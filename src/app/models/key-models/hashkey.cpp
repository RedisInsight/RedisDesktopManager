#include "hashkey.h"
#include "modules/redisclient/command.h"
#include "modules/redisclient/commandexecutor.h"

HashKeyModel::HashKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl)
       : KeyModel(connection, fullPath, dbIndex, ttl, true,
                  "HLEN", "HSCAN %1 0 COUNT 10000", "HGETALL")
{    
}

QString HashKeyModel::getType()
{
    return "hash";
}

QStringList HashKeyModel::getColumnNames()
{
    return QStringList() << "row" << "key" << "value";
}

QHash<int, QByteArray> HashKeyModel::getRoles()
{
    QHash<int, QByteArray> roles;
    roles[Roles::RowNumber] = "row";
    roles[Roles::Key] = "key";
    roles[Roles::Value] = "value";
    roles[Roles::BinaryValue] = "binary_value";
    return roles;
}

QVariant HashKeyModel::getData(int rowIndex, int dataRole)
{
    if (!isRowLoaded(rowIndex))
        return QString();

    QPair<QByteArray, QByteArray> row = m_rowsCache[rowIndex];

    if (dataRole == Roles::Key)
        return row.first;
    else if (dataRole == Roles::Value)
        return row.second;
    else if (dataRole == Roles::RowNumber)
        return QString::number(rowIndex+1);
    else if (dataRole == Roles::BinaryValue)
        return valueToBinary(row.second);

    return QVariant();
}

void HashKeyModel::updateRow(int rowIndex, const QVariantMap &row)
{
    if (!isRowLoaded(rowIndex) || !isRowValid(row))
        throw Exception("Invalid row");

    QPair<QByteArray, QByteArray> cachedRow = m_rowsCache[rowIndex];

    bool keyChanged = cachedRow.first != row["key"].toString();
    bool valueChanged = cachedRow.second != row["value"].toString();

    QPair<QByteArray, QByteArray> newRow(
                    (keyChanged) ? row["key"].toByteArray() : cachedRow.first,
                    (valueChanged) ? row["value"].toByteArray() : cachedRow.second
                );

    if (keyChanged) {
        deleteHashRow(cachedRow.first);
    }

    setHashRow(newRow.first, newRow.second);
    m_rowsCache.replace(rowIndex, newRow);
}

void HashKeyModel::addRow(const QVariantMap &row)
{
    if (!isRowValid(row))
        throw Exception("Invalid row");

    setHashRow(row["key"].toByteArray(), row["value"].toByteArray(), false);
    m_rowCount++;
}

void HashKeyModel::removeRow(int i)
{
    if (!isRowLoaded(i))
        return;

    QPair<QByteArray, QByteArray> row = m_rowsCache[i];

    deleteHashRow(row.first);

    m_rowCount--;
    m_rowsCache.removeAt(i);
    setRemovedIfEmpty();
}

void HashKeyModel::setHashRow(const QByteArray &hashKey, const QByteArray &hashValue,
                              bool updateIfNotExist)
{
    using namespace RedisClient;

    QString command = (updateIfNotExist)? "HSET" : "HSETNX";

    Command addCmd(m_dbIndex); 
    (addCmd << command << m_keyFullPath)
            .append(hashKey)
            .append(hashValue);

    Response result;
    try {
        result = CommandExecutor::execute(m_connection, addCmd);
    } catch (const RedisClient::CommandExecutor::Exception& e) {
        throw Exception("Connection error: " + QString(e.what()));
    }

    if (updateIfNotExist == false
            && result.getValue().toInt() == 0)
        throw Exception("Value with same key already exist");
}

void HashKeyModel::deleteHashRow(const QByteArray &hashKey)
{
    using namespace RedisClient;
    Command deleteCmd(m_dbIndex);
    (deleteCmd << "HDEL" << m_keyFullPath).append(hashKey);

    try {
        CommandExecutor::execute(m_connection, deleteCmd);
    } catch (const RedisClient::CommandExecutor::Exception& e) {
        throw Exception("Connection error: " + QString(e.what()));
    }
}

void HashKeyModel::addLoadedRowsToCache(const QVariantList &rows, int rowStart)
{
    unsigned int rowIndex = rowStart;

    for (QVariantList::const_iterator item = rows.begin();
         item != rows.end(); ++item, rowIndex++) {

        QPair<QByteArray, QByteArray> value;
        value.first = item->toByteArray();
        ++item;

        if (item == rows.end())
            throw Exception("Partial data loaded from server");

        value.second = item->toByteArray();
        m_rowsCache.push_back(value);
    }
}
