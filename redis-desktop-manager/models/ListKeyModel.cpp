#include "ListKeyModel.h"

ListKeyModel::ListKeyModel(RedisClient::Connection * db, const QString &keyName, int dbIndex)
    : PaginatedModel(db, keyName, dbIndex)
{
    setColumnCount(1);
}

void ListKeyModel::loadValue()
{
    QStringList command;
    command << "LRANGE" << keyName <<"0" << "-1";

    db->runCommand(RedisClient::Command(command, this, "loadedValue", dbIndex));
}

void ListKeyModel::setCurrentPage(int page)
{
    if (page == currentPage) {
        return;
    }

    clear();

    QStringList labels("Value");    
    setHorizontalHeaderLabels(labels);

    currentPage = page;

    int size = rawData->size();

    setRowCount( (itemsOnPageLimit > size)? size : itemsOnPageLimit);

    int startShiftPosition = itemsOnPageLimit * (currentPage - 1);
    int limit = startShiftPosition  + itemsOnPageLimit;

    for (int i = startShiftPosition, row = 0; i < limit && i < size; ++i, ++row) {

        QStandardItem * value = new QStandardItem(rawData->at(i));
        value->setData(QVariant(i), KeyModel::KEY_VALUE_TYPE_ROLE);

        setItem(row, 0, value);
    }
}

void ListKeyModel::updateValue(const QString& value, const QModelIndex *cellIndex)
{
    QStandardItem * item = itemFromIndex(*cellIndex);
    item->setText(value);
    int itemIndex = item->data(KeyModel::KEY_VALUE_TYPE_ROLE).toInt();

    QStringList addNew; 
    addNew << "LSET" << keyName << QString::number(itemIndex) << value;

    db->runCommand(RedisClient::Command(addNew, this, "loadedUpdateStatus", dbIndex));
}

void ListKeyModel::loadedUpdateStatus(RedisClient::Response result)
{
    if (result.isErrorMessage()) 
    {
        emit valueUpdateError(result.getValue().toString());
    }
    else 
    {
        emit valueUpdated();    
    }
}
