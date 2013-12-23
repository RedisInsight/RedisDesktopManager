#include "RedisKeyItem.h"
#include "RedisServerItem.h"
#include "RedisServerDbItem.h"
#include "KeyModel.h"
#include "Command.h"
#include "StringKeyModel.h"
#include "HashKeyModel.h"
#include "ListKeyModel.h"
#include "SetKeyModel.h"
#include "SortedSetKeyModel.h"


RedisKeyItem::RedisKeyItem(QString name, RedisServerDbItem * db, const QIcon & icon)
    : ItemWithNaturalSort(icon, name), db(db)
{
    setEditable(false);
}

RedisKeyItem::RedisKeyItem()
    : db(nullptr)
{
    setEditable(false);    
}

RedisServerDbItem * RedisKeyItem::getDbItem()
{
    return this->db;
}

void RedisKeyItem::init(QString name, RedisServerDbItem * db, const QIcon &icon)
{
    setText(name);
    setIcon(icon);

    this->db = db;
}

int RedisKeyItem::type() const
{
    return TYPE;
}

QString RedisKeyItem::getTabLabelText() 
{
    int dbIndex = db->getDbIndex();
    QString dbIndexString = QString::number(dbIndex);
    QString connection = db->server->connection->getConfig().name;

    return QString("%1:%2>%3").arg(connection).arg(dbIndexString).arg(this->text());
}

KeyModel * RedisKeyItem::getKeyModel(const QString & type)
{
    int dbIndex = db->getDbIndex();
    QString keyName = text();

    if (type == "string")
        return new StringKeyModel(db->server->connection, keyName, dbIndex);
    else if (type == "hash") 
        return new HashKeyModel(db->server->connection, keyName, dbIndex);
    else if (type == "list")    
        return new ListKeyModel(db->server->connection, keyName, dbIndex);    
    else if (type == "set")     
        return new SetKeyModel(db->server->connection, keyName, dbIndex);        
    else if (type == "zset") 
        return new SortedSetKeyModel(db->server->connection, keyName, dbIndex);        

    return nullptr;
}

Command RedisKeyItem::getTypeCommand()
{
    QStringList typeCommand;
    typeCommand << "TYPE" << text();

    return Command(typeCommand, nullptr, db->getDbIndex());
}

ConnectionBridge * RedisKeyItem::getConnection()
{
    return db->server->connection;
}

void RedisKeyItem::remove()
{
    db->decreaseKeyCounter();
    db->removeRow(this->row());
}