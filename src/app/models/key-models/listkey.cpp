#include "listkey.h"
#include "modules/redisclient/command.h"
#include "modules/redisclient/commandexecutor.h"

ListKeyModel::ListKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl)
    : KeyModel(connection, fullPath, dbIndex, ttl)
{    
    loadRowCount();
}

QString ListKeyModel::getType()
{
    return "list";
}

QStringList ListKeyModel::getColumnNames()
{
    return QStringList() << "Value";
}

QHash<int, QByteArray> ListKeyModel::getRoles()
{
    QHash<int, QByteArray> roles;
    roles[Roles::Value] = "value";
    return roles;
}

QVariant ListKeyModel::getData(int rowIndex, int dataRole)
{
    if (!isRowLoaded(rowIndex) || dataRole != Roles::Value)
        return QVariant();

    return m_rowsCache[rowIndex];
}

void ListKeyModel::setData(int rowIndex, int dataRole, QString value)
{

}

void ListKeyModel::addRow()
{

}

unsigned long ListKeyModel::rowsCount()
{
    return m_rowCount;
}

void ListKeyModel::loadRows(unsigned long rowStart, unsigned long count, std::function<void ()> callback)
{
    if (isPartialLoadingSupported()) {
        //TBD
    } else {        
        QStringList rows = getRowsRange("LRANGE", rowStart, count).toStringList();

        unsigned int rowIndex = rowStart;

        foreach (QString row, rows) {
            m_rowsCache[rowIndex] = row.toUtf8();
            rowIndex++;
        }
    }

    callback();
}

void ListKeyModel::clearRowCache()
{
    m_rowsCache.clear();
}

void ListKeyModel::removeRow(int)
{

}

bool ListKeyModel::isRowLoaded(int rowIndex)
{
    return m_rowsCache.contains(rowIndex);
}

bool ListKeyModel::isMultiRow() const
{
    return true;
}

void ListKeyModel::loadRowCount()
{    
    m_rowCount = getRowCount("LLEN");
}

//void ListKeyModel::setCurrentPage(int page)
//{
//    if (page == currentPage) {
//        return;
//    }

//    clear();

//    QStringList labels("Value");
//    setHorizontalHeaderLabels(labels);

//    currentPage = page;

//    int size = rawData->size();

//    setRowCount( (itemsOnPageLimit > size)? size : itemsOnPageLimit);

//    int startShiftPosition = itemsOnPageLimit * (currentPage - 1);
//    int limit = startShiftPosition  + itemsOnPageLimit;

//    for (int i = startShiftPosition, row = 0; i < limit && i < size; ++i, ++row) {

//        QStandardItem * value = new QStandardItem(rawData->at(i));
//        value->setData(QVariant(i), KeyModel::KEY_VALUE_TYPE_ROLE);

//        setItem(row, 0, value);
//    }
//}

//void ListKeyModel::updateValue(const QString& value, const QModelIndex *cellIndex)
//{
//    QStandardItem * item = itemFromIndex(*cellIndex);
//    item->setText(value);
//    int itemIndex = item->data(KeyModel::KEY_VALUE_TYPE_ROLE).toInt();

//    QStringList addNew;
//    addNew << "LSET" << keyName << QString::number(itemIndex) << value;

//    db->runCommand(RedisClient::Command(addNew, this, "loadedUpdateStatus", dbIndex));
//}


