#include "RedisServerItem.h"



RedisServerItem::RedisServerItem(RedisConnectionAbstract * c) 
	: connection(c), isDbInfoLoaded(false)
{						
	setOfflineIcon();
	getItemNameFromConnection();
	setEditable(false);
}

void RedisServerItem::getItemNameFromConnection()
{
	setText(connection->config.name);
}

void RedisServerItem::setConnection(RedisConnectionAbstract * c)
{
	connection = c;
}

bool RedisServerItem::loadDatabases()
{		
	if (isDbInfoLoaded) return true;

	setBusyIcon();

	if (!connection->isConnected() && !connection->connect()) {
		// TODO : replace this code by bool checkConnection() { if no_connection -> set server in offline state }
		// TODO: set error icon		
		setOfflineIcon();
		return false;
	}

	RedisConnectionAbstract::RedisDatabases databases = connection->getDatabases();

	if (databases.size() == 0) 
	{
		QString error = connection->getLastError();

		if (!error.isEmpty()) {
			// TODO: set error icon
			setText(QString("%1 (error:%2)").arg(connection->config.name).arg(error));
		}
		setNormalIcon();
		return false;
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

	return true;
}

QStringList RedisServerItem::getInfo()
{
	if (!connection->isConnected() && !connection->connect()) {
		// TODO : replace this code by bool checkConnection() { if no_connection -> set server in offline state }
		// TODO: set error icon		
		setOfflineIcon();
		return QStringList();
	}

	QVariant info = connection->execute("INFO");

	if (info.isNull()) {
		return QStringList();
	}

	return info.toString().split("\r\n");
}

RedisConnectionAbstract * RedisServerItem::getConnection()
{
	return connection;
}

void RedisServerItem::reload()
{
	unload();

	loadDatabases();
}

void RedisServerItem::unload()
{
	removeRows(0, rowCount());

	isDbInfoLoaded = false;

	getItemNameFromConnection();
}


void RedisServerItem::setBusyIcon()
{
	setIcon(QIcon(":/images/wait.png"));
}

void RedisServerItem::setNormalIcon()
{
	setIcon(QIcon(":/images/redisIcon.png"));
}

void RedisServerItem::setOfflineIcon()
{
	setIcon(QIcon(":/images/redisIcon_offline.png"));
}

int RedisServerItem::type() const
{
	return TYPE;
}