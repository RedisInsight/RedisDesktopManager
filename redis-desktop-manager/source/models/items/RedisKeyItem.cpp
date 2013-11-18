#include "RedisKeyItem.h"
#include "RedisServerItem.h"
#include "RedisServerDbItem.h"

RedisKeyItem::RedisKeyItem(QString name, RedisServerDbItem * db, const QIcon & icon)
	: ItemWithNaturalSort(icon, name), db(db), fullName(name), keyType(Empty)
{
	setEditable(false);
}

RedisKeyItem::Type RedisKeyItem::getKeyType()
{
	if (keyType != Empty) {
		return keyType;
	}

	QEventLoop loop;
	QTimer timer;

	timer.setSingleShot(true);
	connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
	connect(this, SIGNAL(keyTypeLoaded()), &loop, SLOT(quit()));
	connect(db->server->connection, SIGNAL(responseResieved(const QVariant&, QObject *)),
		this, SLOT(loadedType(const QVariant&, QObject*)));

	timer.start(db->server->connection->getConfig().executeTimeout);
	db->server->connection->addCommand(Command(QString("type %1").arg(fullName), this, db->getDbIndex()));
	loop.exec();

	return keyType;
}

void RedisKeyItem::getValue()
{
 	if (keyType == Empty) {
 		getKeyType();
 	}

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
		emit valueLoaded(QVariant(), this);
		return;
	} else {
		connect(connection, SIGNAL(responseResieved(const QVariant&, QObject *)),
			this, SIGNAL(valueLoaded(const QVariant&, QObject*)));

		connection->addCommand(Command(command, this, db->getDbIndex()));
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
	QString connection = db->server->connection->getConfig().name;

	return QString("%1:%2>%3").arg(connection).arg(dbIndexString).arg(this->text());
}

QString RedisKeyItem::getFullName()
{
	return fullName;
}

void RedisKeyItem::loadedValue(const QVariant& value, QObject *sender)
{
	if (sender != this) {
		return;
	}

	db->server->connection->disconnect(this);

	emit valueLoaded(value, this);
}

void RedisKeyItem::loadedType(const QVariant& result, QObject * owner)
{
	if (owner != this) {
		return;
	}

	db->server->connection->disconnect(this);

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

	emit keyTypeLoaded();

	return;
}