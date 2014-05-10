#include "SetKeyModel.h"

SetKeyModel::SetKeyModel(RedisClient::Connection * db, const QString &keyName, int dbIndex)
    : ListKeyModel(db, keyName, dbIndex)
{
}

void SetKeyModel::loadValue()
{
    QStringList command;
    command << "SMEMBERS" << keyName;

    db->runCommand(RedisClient::Command(command, this, "loadedValue", dbIndex));
}

void SetKeyModel::updateValue(const QString& value, const QModelIndex *cellIndex)
{
    QStandardItem * item = itemFromIndex(*cellIndex);
    QString oldValue = item->text();
    item->setText(value);

    QString deleteOld = QString("SREM %1 %2")
        .arg(keyName)
        .arg(oldValue);

    db->runCommand(RedisClient::Command(deleteOld, this, dbIndex));

    QString addNew = QString("SADD %1 %2")
        .arg(keyName)
        .arg(value);

    db->runCommand(RedisClient::Command(addNew, this, "loadedUpdateStatus", dbIndex));
}

void SetKeyModel::loadedUpdateStatus(RedisClient::Response result)
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
