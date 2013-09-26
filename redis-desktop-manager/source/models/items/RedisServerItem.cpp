#include "RedisServerItem.h"



RedisServerItem::RedisServerItem(RedisConnectionAbstract * c) 
	: connection(c), isDbInfoLoaded(false)
{						
	setOfflineIcon();
	setText(connection->config.name);
	setEditable(false);
}

void RedisServerItem::loadDatabases()
{		
	if (isDbInfoLoaded) return;

	if (!connection->isConnected() && !connection->connect()) {
		// TODO : replace this code by bool checkConnection() { if no_connection -> set server in offline state }
		// TODO: set error icon		

		return;
	}

	setBusyIcon();

	RedisConnectionAbstract::RedisDatabases databases = connection->getDatabases();

	if (databases.size() == 0) 
	{
		QString error = connection->getLastError();

		if (!error.isEmpty()) {
			// TODO: set error icon
			setText(QString("%1 (error:%2)").arg(connection->config.name).arg(error));
		}
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
}

void RedisServerItem::reload()
{
	removeRows(0, rowCount());

	isDbInfoLoaded = false;

	loadDatabases();
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