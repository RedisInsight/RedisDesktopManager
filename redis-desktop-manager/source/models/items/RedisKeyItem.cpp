#include "RedisKeyItem.h"
#include "RedisServerItem.h"
#include "RedisServerDbItem.h"

RedisKeyItem::RedisKeyItem(QString name, RedisServerDbItem * db, const QIcon & icon)
	: QStandardItem(icon, name), db(db), fullName(name), keyType(Empty)
{
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

void RedisKeyItem::getValue()
{
	if (keyType == Empty) {
		getKeyType();
	}

	db->setCurrent();

	auto connection = db->server->connection;

	QString command;

	switch (keyType)
	{
	case RedisKeyItem::String:
		command = QString("get %1").arg(fullName);
		break;
		
	case RedisKeyItem::Hash:		
		command = QString("hgetall %1").arg(fullName);
		break;

	case RedisKeyItem::List:
		command = QString("LRANGE %1 0 -1").arg(fullName);		
		break;

	case RedisKeyItem::Set:
		command = QString("SMEMBERS %1").arg(fullName);				
		break;

	case RedisKeyItem::ZSet:		
		command = QString("ZRANGE %1 0 -1 WITHSCORES").arg(fullName);
		break;
	}	

	if (command.isEmpty()) {
		emit valueLoaded(QVariant());
		return;
	} else {
		connect(connection, SIGNAL(responseResived(QVariant&)),
			this, SIGNAL(valueLoaded(QVariant&)));

		connection->runCommand(command, db->getDbIndex());
	}
}

int RedisKeyItem::type() const
{
	return TYPE;
}

QString RedisKeyItem::getFullText() 
{
	int dbIndex = db->getDbIndex();
	QString dbIndexString = QString::number(dbIndex);
	QString connection = db->server->connection->config.name;

	return QString("%1:%2>%3").arg(connection).arg(dbIndexString).arg(this->text());
}

QString RedisKeyItem::getFullName()
{
	return fullName;
}