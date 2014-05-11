#include "RedisServerItem.h"
#include "connectionconfig.h"

RedisServerItem::RedisServerItem(RedisClient::Connection * c)
    : connection(c), isDbInfoLoaded(false), locked(false)
{                        
    setOfflineIcon();
    getItemNameFromConnection();
    setEditable(false);
    connect(connection, SIGNAL(error(QString)), this, SLOT(proccessError(QString)));
}

void RedisServerItem::getItemNameFromConnection()
{
    setText(connection->getConfig().name);
}

void RedisServerItem::setConnection(RedisClient::Connection * c)
{
    connection = c;
    connect(connection, SIGNAL(error(QString)), this, SLOT(proccessError(QString)));
}

void RedisServerItem::loadDatabaseList()
{        
    if (isDbInfoLoaded || locked) 
        return;

    setBusyIcon();
    getItemNameFromConnection();   

    if (!connection->connect()) {
        setOfflineIcon();
        return;
    }

    auto databases = connection->operations()->getDatabases();

    if (databases.size() == 0)
    {
        setNormalIcon();
        return;
    }   

    QMap<QString, int>::const_iterator db = databases.constBegin();

    while (db != databases.constEnd()) {
        QStandardItem * newDb = new RedisServerDbItem(db.key(), db.value(), this);
        appendRow(newDb);
        ++db;
    }

    sortChildren(0);
    setNormalIcon();

    isDbInfoLoaded = true;

    emit databasesLoaded();
    emit unlockUI();
}


QStringList RedisServerItem::getInfo()
{
    if (!connection->connect()) {
        setOfflineIcon();
        return QStringList();
    }

    return connection->operations()->getInfo();
}

void RedisServerItem::proccessError(QString srcError)
{    
    setOfflineIcon();
    locked = false;

    QString message = QString("Connection Error (%1): %2")
        .arg(text())
        .arg(srcError);

    emit error(message);
}

RedisClient::Connection * RedisServerItem::getConnection()
{
    return connection;
}

void RedisServerItem::reload()
{
    blockSignals(true);
    unload();
    blockSignals(false);

    loadDatabaseList();
}

void RedisServerItem::unload()
{
    setBusyIcon();

    removeRows(0, rowCount());

    isDbInfoLoaded = false;

    getItemNameFromConnection();

    setOfflineIcon();

    emit unlockUI();
}

void RedisServerItem::setBusyIcon()
{
    locked = true;
    setIcon(QIcon(":/images/wait.png"));
}

void RedisServerItem::setNormalIcon()
{
    emit unlockUI();
    locked = false;
    setIcon(QIcon(":/images/redisIcon.png"));
}

void RedisServerItem::setOfflineIcon()
{
    emit unlockUI();
    locked = false;
    setIcon(QIcon(":/images/redisIcon_offline.png"));
}

int RedisServerItem::type() const
{
    return TYPE;
}

bool RedisServerItem::isLocked()
{
    return locked;
}
