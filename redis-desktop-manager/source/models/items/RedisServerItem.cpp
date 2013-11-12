#include "RedisServerItem.h"
#include "RedisConnectionConfig.h"

RedisServerItem::RedisServerItem(ConnectionBridge * c) 
	: connection(c), isDbInfoLoaded(false)
{						
	setOfflineIcon();
	getItemNameFromConnection();
	setEditable(false);
	
	connect(c, SIGNAL(dbListLoaded(RedisConnectionAbstract::RedisDatabases)),
		this, SLOT(databaseDataLoaded(RedisConnectionAbstract::RedisDatabases)));
}

void RedisServerItem::getItemNameFromConnection()
{
	setText(connection->getConfig().name);
}

void RedisServerItem::setConnection(ConnectionBridge * c)
{
	connection = c;
}

void RedisServerItem::runDatabaseLoading()
{		
	if (isDbInfoLoaded) return;

	setBusyIcon();

// 	if (!connection->isConnected() && !connection->connect()) {
// 		setOfflineIcon();
// 		emit error(QString("Error occurred on database load: %1").arg(connection->getLastError()));
// 		return;
// 	}

	connection->initWorker();
	connection->loadDatabasesList();
}

void RedisServerItem::databaseDataLoaded(RedisConnectionAbstract::RedisDatabases databases)
{
	if (databases.size() == 0) 
	{
// 		QString errorMsg = connection->getLastError();
// 
// 		if (!errorMsg.isEmpty()) {		
// 			emit error(QString("Error occurred on database load: %1").arg(errorMsg));
// 		}

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
}

QStringList RedisServerItem::getInfo()
{
// 	if (!connection->isConnected() && !connection->connect()) {
// 		// TODO : replace this code by bool checkConnection() { if no_connection -> set server in offline state }
// 		// TODO: set error icon		
// 		setOfflineIcon();
// 		return QStringList();
// 	}
// 
// 	QVariant info = connection->execute("INFO");
// 
// 	if (info.isNull()) {
// 		return QStringList();
// 	}
// 
// 	return info.toString().split("\r\n");

	return QStringList();
}

ConnectionBridge * RedisServerItem::getConnection()
{
	return connection;
}

void RedisServerItem::reload()
{
	unload();

	runDatabaseLoading();
}

void RedisServerItem::unload()
{
	removeRows(0, rowCount());

	isDbInfoLoaded = false;

	getItemNameFromConnection();

	setOfflineIcon();
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