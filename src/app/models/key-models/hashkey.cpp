#include "hashkey.h"
#include <qredisclient/connection.h>
#include <QObject>

HashKeyModel::HashKeyModel(QSharedPointer<RedisClient::Connection> connection,
                           QByteArray fullPath, int dbIndex, long long ttl)
       : KeyModel(connection, fullPath, dbIndex, ttl, true,
                  "HLEN", "HSCAN %1 0 COUNT 10000")
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
    return roles;
}

QVariant HashKeyModel::getData(int rowIndex, int dataRole)
{
    if (!isRowLoaded(rowIndex))
        return QVariant();

    QPair<QByteArray, QByteArray> row = m_rowsCache[rowIndex];

    if (dataRole == Roles::Key)
        return row.first;
    else if (dataRole == Roles::Value)
        return row.second;
    else if (dataRole == Roles::RowNumber)
        return rowIndex+1;

    return QVariant();
}

void HashKeyModel::updateRow(int rowIndex, const QVariantMap &row)
{
    if (!isRowLoaded(rowIndex) || !isRowValid(row))
        throw Exception(QObject::tr("Invalid row"));

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
        throw Exception(QObject::tr("Invalid row"));

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

    Response result;

    try {        
        result = m_connection->commandSync({(updateIfNotExist)? "HSET" : "HSETNX",
                                            m_keyFullPath, hashKey, hashValue}, m_dbIndex);
    } catch (const RedisClient::Connection::Exception& e) {
        throw Exception(QObject::tr("Connection error: ") + QString(e.what()));
    }

    if (updateIfNotExist == false
            && result.getValue().toInt() == 0)
        throw Exception(QObject::tr("Value with the same key already exist"));
}

void HashKeyModel::deleteHashRow(const QByteArray &hashKey)
{
    try {
        m_connection->commandSync({"HDEL", m_keyFullPath, hashKey}, m_dbIndex);
    } catch (const RedisClient::Connection::Exception& e) {
        throw Exception(QObject::tr("Connection error: ") + QString(e.what()));
    }
}

void HashKeyModel::addLoadedRowsToCache(const QVariantList &rows, int rowStart)
{    
    QList<QPair<QByteArray, QByteArray>> result;

    for (QVariantList::const_iterator item = rows.begin();
         item != rows.end(); ++item) {

        QPair<QByteArray, QByteArray> value;
        value.first = item->toByteArray();
        ++item;

        if (item == rows.end())
            throw Exception(QObject::tr("Data was loaded from server partially."));

        value.second = item->toByteArray();
        result.push_back(value);
    }

    m_rowsCache.addLoadedRange({rowStart, rowStart + result.size() - 1},
                               result);
}
