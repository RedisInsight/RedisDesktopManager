#include "hashkey.h"
#include "modules/redisclient/command.h"
#include "modules/redisclient/commandexecutor.h"

HashKeyModel::HashKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl)
       : KeyModel(connection, fullPath, dbIndex, ttl)
{
    loadRowCount();
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
        return QString();

    QPair<QByteArray, QByteArray> row = m_rowsCache[rowIndex];

    if (dataRole == Roles::Key)
        return row.first;
    else if (dataRole == Roles::Value)
        return row.second;    
    else if (dataRole == Roles::RowNumber)
        return QString::number(rowIndex+1);

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
}

unsigned long HashKeyModel::rowsCount()
{
    return m_rowCount;
}

void HashKeyModel::loadRows(unsigned long rowStart, unsigned long count, std::function<void ()> callback)
{
    if (isPartialLoadingSupported()) {
        //TBD
    } else {
        QStringList rows = getRowsRange("HGETALL").toStringList();

        unsigned int rowIndex = rowStart;

        for (QStringList::iterator item = rows.begin();
             item != rows.end(); ++item, rowIndex++) {

            QPair<QByteArray, QByteArray> value;
            value.first = item->toUtf8();
            ++item;

            if (item == rows.end())
                throw Exception("Partial data loaded from server");

            value.second = item->toUtf8();
            m_rowsCache.push_back(value);
        }
    }

    callback();
}

void HashKeyModel::clearRowCache()
{
    m_rowsCache.clear();
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

bool HashKeyModel::isRowLoaded(int rowIndex)
{
    return 0 <= rowIndex && rowIndex < m_rowsCache.size();
}

bool HashKeyModel::isMultiRow() const
{
    return true;
}

void HashKeyModel::loadRowCount()
{
    m_rowCount = getRowCount("HLEN");   
}

void HashKeyModel::setHashRow(const QString &hashKey, const QString &hashValue,
                              bool updateIfNotExist)
{
    using namespace RedisClient;

    QString command = (updateIfNotExist)? "HSET" : "HSETNX";

    Command addCmd(QStringList() << command << m_keyFullPath << hashKey << hashValue,
                   m_dbIndex);

    Response result = CommandExecutor::execute(m_connection, addCmd);

    if (updateIfNotExist == false
            && result.getValue().toInt() == 0)
        throw Exception("Value with same key already exist");
}

void HashKeyModel::deleteHashRow(const QString &hashKey)
{
    using namespace RedisClient;
    Command deleteCmd(QStringList()<< "HDEL" << m_keyFullPath << hashKey, m_dbIndex);
    CommandExecutor::execute(m_connection, deleteCmd);
}

