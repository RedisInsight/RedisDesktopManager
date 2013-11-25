#include "RedisKeyItem.h"
#include "RedisServerItem.h"
#include "RedisServerDbItem.h"
#include "KeyModel.h"

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

KeyModel * RedisKeyItem::getKeyModel()
{
	int dbIndex = db->getDbIndex();
	QString keyName = text();
	return new KeyModel(db->server->connection, keyName, dbIndex);
}