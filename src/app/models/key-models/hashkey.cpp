#include "hashkey.h"
#include <QStandardItem>

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
    return QStringList() << "Key" << "Value";
}

QHash<int, QByteArray> HashKeyModel::getRoles()
{
    QHash<int, QByteArray> roles;
    roles[Roles::Key] = "key";
    roles[Roles::Value] = "value";
    return roles;
}

QString HashKeyModel::getData(int rowIndex, int dataRole)
{
    if (!isRowLoaded(rowIndex) || (dataRole != Roles::Value && dataRole != Roles::Key))
        return QString();

    QPair<QByteArray, QByteArray> row = m_rowsCache[rowIndex];

    if (dataRole == Roles::Key)
        return row.first;
    else if (dataRole ==Roles::Value)
        return row.second;

    return QString();
}

void HashKeyModel::setData(int rowIndex, int dataRole, QString value)
{

}

void HashKeyModel::addRow()
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
}

//void HashKeyModel::setCurrentPage(int page)
//{
//    if (page == currentPage) {
//        return;
//    }

//    clear();

//    QStringList labels;
//    labels << "Hash Key" << "Hash Value";
//    setHorizontalHeaderLabels(labels);

//    currentPage = page;

//    int size = rawData->size();

//    setRowCount( (itemsOnPageLimit > size / 2)? size / 2 : itemsOnPageLimit);
    
//    int startShiftPosition = itemsOnPageLimit * 2 * (currentPage - 1);
//    int limit = startShiftPosition + itemsOnPageLimit * 2;

//    for (int i = startShiftPosition, row = 0; i < limit && i < size; ++i, ++row) {

//        QStandardItem * key = new QStandardItem(rawData->at(i));
//        key->setData(QVariant("key"), KeyModel::KEY_VALUE_TYPE_ROLE);

//        QStandardItem * value = new QStandardItem(rawData->at(++i));
//        value->setData(QVariant("value"), KeyModel::KEY_VALUE_TYPE_ROLE);

//        setItem(row, 0, key);
//        setItem(row, 1, value);
//    }
//}

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
