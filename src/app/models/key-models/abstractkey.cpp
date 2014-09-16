#include "abstractkey.h"

#include <QVariant>
#include "modules/redisclient/redisclient.h"

KeyModel::KeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl)
    : m_connection(connection), m_keyFullPath(fullPath), m_dbIndex(dbIndex), m_ttl(ttl), m_isKeyRemoved(false)
{

}

QString KeyModel::getKeyName()
{
    return m_keyFullPath;
}

int KeyModel::getTTL()
{
    return m_ttl;
}

bool KeyModel::isPartialLoadingSupported()
{
    return m_connection->getServerVersion() >= 2.8;
}

void KeyModel::setKeyName(const QString &newKeyName)
{
    QStringList renameCommand;

    renameCommand << "RENAME" << m_keyFullPath << newKeyName;

    //db->runCommand(RedisClient::Command(renameCommand, this, "loadedRenameStatus", dbIndex));
    // TBD
}

void KeyModel::setTTL(int)
{
    // TBD
}

void KeyModel::removeKey()
{
    QStringList deleteCommand;

    deleteCommand << "DEL" << m_keyFullPath;

    //db->runCommand(RedisClient::Command(deleteCommand, this, "loadedDeleteStatus", dbIndex));
    //TBD
}


