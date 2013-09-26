#include "RedisKeyItem.h"
#include "RedisServerItem.h"
#include "RedisServerDbItem.h"

RedisKeyItem::RedisKeyItem(QString name, RedisServerDbItem * db)
	: db(db), fullName(name), keyType(Empty)
{
	setNormalIcon();
	setText(name);
	setEditable(false);
}

RedisKeyItem::Type RedisKeyItem::getKeyType()
{
	db->setCurrent();

	auto connection = db->server->connection;
	QVariant result = connection->execute( QString("type %1").arg(fullName));

	QString t = result.toString();

	keyType = None;

	if (t == "string")
		keyType = String;

	if (t == "hash") 
		keyType = Hash;

	if (t == "list")
		keyType = List;

	if (t == "set") 
		keyType = Set;

	if (t == "zset") 
		keyType = ZSet;

	return keyType;
}

QVariant RedisKeyItem::getValue()
{
	if (keyType == Empty) {
		getKeyType();
	}

	db->setCurrent();

	auto connection = db->server->connection;

	switch (keyType)
	{
	case RedisKeyItem::String:

		return connection->execute(QString("get %1").arg(fullName));
		
	case RedisKeyItem::Hash:
		
		return connection->execute(QString("hgetall %1").arg(fullName));

	case RedisKeyItem::List:
		return connection->execute(QString("LRANGE %1 0 -1").arg(fullName));		

	case RedisKeyItem::Set:
		return connection->execute(QString("SMEMBERS %1").arg(fullName));				

	case RedisKeyItem::ZSet:		
		return connection->execute(QString("ZRANGE %1 0 -1 WITHSCORES").arg(fullName));

	case RedisKeyItem::None:
	case RedisKeyItem::Empty:
	default:
		return QVariant();
	}	
}

int RedisKeyItem::type() const
{
	return TYPE;
}

void RedisKeyItem::setBusyIcon()
{
	setIcon(QIcon(":/images/wait.png"));
}

void RedisKeyItem::setNormalIcon()
{
	setIcon(QIcon(":/images/key.png"));
}

QString RedisKeyItem::getFullText() 
{
	int dbIndex = db->getDbIndex();
	QString dbIndexString = QString::number(dbIndex);
	QString connection = db->parent()->text();

	return QString("%1:%2>%3").arg(connection).arg(dbIndexString).arg(this->text());
}