#include "StringKeyModel.h"

StringKeyModel::StringKeyModel(RedisClient::Connection * db, const QString &keyName, int dbIndex)
    : KeyModel(db, keyName, dbIndex)
{
}

void StringKeyModel::loadValue()
{
    QStringList command;
    command << "GET" << keyName;    

    db->runCommand(RedisClient::Command(command, this, "loadedValue", dbIndex));
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

    db->runCommand(RedisClient::Command(updateCommand, this, "loadedUpdateStatus", dbIndex));
}

void StringKeyModel::loadedUpdateStatus(RedisClient::Response result)
{
    if (result.isErrorMessage()) 
    {
        emit valueUpdateError(result.getValue().toString());
    }
    else 
        emit valueUpdated();    
}
