#include "listkey.h"
#include "modules/redisclient/command.h"
#include "modules/redisclient/commandexecutor.h"

ListKeyModel::ListKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl)
    : ListLikeKeyModel(connection, fullPath, dbIndex, ttl)
{    
    loadRowCount();
}

QString ListKeyModel::getType()
{
    return "list";
}

void ListKeyModel::setData(int rowIndex, int dataRole, QString value)
{

}

void ListKeyModel::addRow(const QVariantMap &)
{

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

void ListKeyModel::removeRow(int)
{

}

void ListKeyModel::loadRowCount()
{    
    m_rowCount = getRowCount("LLEN");
}

//void ListKeyModel::updateValue(const QString& value, const QModelIndex *cellIndex)
//{
//    QStandardItem * item = itemFromIndex(*cellIndex);
//    item->setText(value);
//    int itemIndex = item->data(KeyModel::KEY_VALUE_TYPE_ROLE).toInt();

//    QStringList addNew;
//    addNew << "LSET" << keyName << QString::number(itemIndex) << value;

//    db->runCommand(RedisClient::Command(addNew, this, "loadedUpdateStatus", dbIndex));
//}


