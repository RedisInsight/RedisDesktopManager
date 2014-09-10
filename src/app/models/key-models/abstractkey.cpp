#include "abstractkey.h"

#include <QVariant>
#include "modules/redisclient/redisclient.h"

KeyModel::KeyModel(RedisClient::Connection * db, const QString &keyName, int dbIndex)
    : db(db), keyName(keyName), dbIndex(dbIndex)
{    
}

QString KeyModel::getKeyName()
{
    return keyName;
}

void KeyModel::loadedValue(RedisClient::Response value)
{
    initModel(value.getValue());

    emit valueLoaded();
}

void KeyModel::renameKey(const QString& newKeyName)
{    
    QStringList renameCommand;

    renameCommand << "RENAME" << keyName  << newKeyName;
    
    db->runCommand(RedisClient::Command(renameCommand, this, "loadedRenameStatus", dbIndex));
}

void KeyModel::loadedRenameStatus(RedisClient::Response result)
{
    if (result.isErrorMessage()) 
        emit keyRenameError(result.getValue().toString());
    else 
        emit keyRenamed();    
}

void KeyModel::loadTTL()
{
    QStringList ttlCommand;

    ttlCommand << "TTL" << keyName;

    db->runCommand(RedisClient::Command(ttlCommand, this, "ttlLoaded", dbIndex));
}

void KeyModel::deleteKey()
{
    QStringList deleteCommand;
    
    deleteCommand << "DEL" << keyName;

    db->runCommand(RedisClient::Command(deleteCommand, this, "loadedDeleteStatus", dbIndex));
}

void KeyModel::loadedDeleteStatus(RedisClient::Response result)
{
    if (result.isErrorMessage()) 
    {
        emit keyDeleteError(result.getValue().toString());
    }
    else 
        emit keyDeleted();    
}

KeyModel::~KeyModel()
{
}
