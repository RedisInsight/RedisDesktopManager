#include "HashKeyModel.h"
#include <QStandardItem>

HashKeyModel::HashKeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex)
    : PaginatedModel(db, keyName, dbIndex)
{
    setColumnCount(2);
}

void HashKeyModel::loadValue()
{
    QStringList command;
    command << "hgetall" << keyName;

    db->addCommand(Command(command, this, CALLMETHOD("loadedValue"), dbIndex));
}

void HashKeyModel::setCurrentPage(int page)
{
    if (page == currentPage) {
        return;
    }

    clear();

    QStringList labels;
    labels << "Hash Key" << "Hash Value";
    setHorizontalHeaderLabels(labels);

    currentPage = page;

    int size = rawData->size();

    setRowCount( (itemsOnPageLimit > size / 2)? size / 2 : itemsOnPageLimit);
    
    int startShiftPosition = itemsOnPageLimit * 2 * (currentPage - 1);
    int limit = startShiftPosition + itemsOnPageLimit * 2;

    for (int i = startShiftPosition, row = 0; i < limit && i < size; ++i, ++row) {

        QStandardItem * key = new QStandardItem(rawData->at(i));
        key->setData(QVariant("key"), KeyModel::KEY_VALUE_TYPE_ROLE);

        QStandardItem * value = new QStandardItem(rawData->at(++i));
        value->setData(QVariant("value"), KeyModel::KEY_VALUE_TYPE_ROLE);

        setItem(row, 0, key);
        setItem(row, 1, value);
    }
}

int HashKeyModel::itemsCount()
{
    return rawData->size() / 2;
}

void HashKeyModel::updateValue(const QString& value, const QModelIndex *cellIndex)
{
    QStandardItem * currentItem = itemFromIndex(*cellIndex);    

    QString itemType = currentItem->data(KeyModel::KEY_VALUE_TYPE_ROLE).toString();

    if (itemType == "key") 
    {
        QStringList removeCmd;
        removeCmd << "HDEL"
                  << keyName
                  << currentItem->text();        

        db->addCommand(Command(removeCmd, this, dbIndex));

        QStandardItem * valueItem = item(currentItem->row(), 1);

        QStringList addCmd;

        addCmd << "HSET"
               << keyName
               << value
               << valueItem->text();


        db->addCommand(Command(addCmd, this, CALLMETHOD("loadedUpdateStatus"), dbIndex));

    } else if (itemType == "value") {

        QStandardItem * keyItem = item(currentItem->row(), 0);

        QStringList setCmd;

        setCmd << "HSET"
            << keyName
            << keyItem->text()
            << value;

        db->addCommand(Command(setCmd, this, CALLMETHOD("loadedUpdateStatus"), dbIndex));
    }

    currentItem->setText(value);
}

void HashKeyModel::loadedUpdateStatus(Response result)
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