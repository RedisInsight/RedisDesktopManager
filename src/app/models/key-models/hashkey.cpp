#include "hashkey.h"
#include <QStandardItem>
#include <QDebug>

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

void HashKeyModel::updateRow(int rowIndex, const QVariantMap &)
{

}

void HashKeyModel::addRow(const QVariantMap &)
{

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
            m_rowsCache[rowIndex] = value;
        }
    }

    callback();
}

void HashKeyModel::clearRowCache()
{

}

void HashKeyModel::removeRow(int)
{

}

bool HashKeyModel::isRowLoaded(int rowIndex)
{
    return m_rowsCache.contains(rowIndex);
}

bool HashKeyModel::isMultiRow() const
{
    return true;
}

void HashKeyModel::loadRowCount()
{
    m_rowCount = getRowCount("HLEN");

    qDebug() << "row count:" << m_rowCount;
}

//void HashKeyModel::updateValue(const QString& value, const QModelIndex *cellIndex)
//{
//    QStandardItem * currentItem = itemFromIndex(*cellIndex);

//    QString itemType = currentItem->data(KeyModel::KEY_VALUE_TYPE_ROLE).toString();

//    if (itemType == "key")
//    {
//        QStringList removeCmd;
//        removeCmd << "HDEL"
//                  << keyName
//                  << currentItem->text();

//        db->runCommand(RedisClient::Command(removeCmd, this, dbIndex));

//        QStandardItem * valueItem = item(currentItem->row(), 1);

//        QStringList addCmd;

//        addCmd << "HSET"
//               << keyName
//               << value
//               << valueItem->text();


//        db->runCommand(RedisClient::Command(addCmd, this, "loadedUpdateStatus", dbIndex));

//    } else if (itemType == "value") {

//        QStandardItem * keyItem = item(currentItem->row(), 0);

//        QStringList setCmd;

//        setCmd << "HSET"
//            << keyName
//            << keyItem->text()
//            << value;

//        db->runCommand(RedisClient::Command(setCmd, this, "loadedUpdateStatus", dbIndex));
//    }

//    currentItem->setText(value);
//}
