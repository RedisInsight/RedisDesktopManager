#include "StringKeyModel.h"

StringKeyModel::StringKeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex)
    : KeyModel(db, keyName, dbIndex)
{
}

void StringKeyModel::loadValue()
{
    QStringList command;
    command << "GET" << keyName;    

    db->addCommand(Command(command, this, CALLMETHOD("loadedValue"), dbIndex));
}

QString StringKeyModel::getValue()
{
    return plainData;
}

void StringKeyModel::initModel(const QVariant &value)
{
    plainData = value.toString();
}

void StringKeyModel::updateValue(const QString& value, const QModelIndex *cellIndex)
{
    Q_UNUSED(cellIndex);

    QStringList updateCommand;
    updateCommand << "SET" << keyName << value;    

    db->addCommand(Command(updateCommand, this, CALLMETHOD("loadedUpdateStatus"), dbIndex));
}

void StringKeyModel::loadedUpdateStatus(Response result)
{
    if (result.isErrorMessage()) 
    {
        emit valueUpdateError(result.getValue().toString());
    }
    else 
        emit valueUpdated();    
}
