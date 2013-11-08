#include "RedisConnectionAbstract.h"

RedisConnectionAbstract::RedisConnectionAbstract(const RedisConnectionConfig & c) 
	: config(c), connected(false), commandRunning(false), keysLoadingRunning(false)  
{
	executionTimer.setSingleShot(true);
	QObject::connect(&executionTimer, SIGNAL(timeout()), this, SLOT(executionTimeout()));
};

void RedisConnectionAbstract::getDatabases()
{
	RedisDatabases availableDatabeses;

	QVariant rawDbCount = execute("config get databases");

	QStringList dbInfo = rawDbCount.toStringList();

	int dbCount = 0;

	//redis >=2.6 & config commands allowed by configuration
	if (dbInfo.size() == 2) {
		dbCount = dbInfo.at(1).toInt();		
	} else { //cloud & legacy redis versions

		QString scanningResp;

		do {			
			scanningResp = execute(QString("select %1").arg(dbCount)).toString();	

		} while (scanningResp == "OK" && ++dbCount);
	}	

	// build db list
	for (int dbIndex = 0; dbIndex < dbCount; ++dbIndex)
	{
		availableDatabeses.insert(QString("db%1").arg(dbIndex), 0);
	}

	//	Get keys count		
	QVariant result = execute("INFO keyspace");

	if (result.isNull()) {
		emit databesesLoaded(availableDatabeses);
		return;
	}

	QStringList keyspaceInfo = result.toString().split("\r\n", QString::SkipEmptyParts);

	QRegExp getDbAndKeysCount("(db\\d+):keys=(\\d+),expires=(\\d+)");

	for (QString line : keyspaceInfo) {

		if (!line.contains(':')) // skip header  
			continue;

		int pos = getDbAndKeysCount.indexIn(line);

		if (pos == -1) //skip "bad" lines 
			continue;

		QString dbName = getDbAndKeysCount.cap(1);
		int keysCount = getDbAndKeysCount.cap(2).toInt();

		availableDatabeses[dbName] = keysCount;
	}

	emit databesesLoaded(availableDatabeses);
	return;
} 

void RedisConnectionAbstract::selectDb(int dbIndex)
{
	execute(QString("select %1").arg(dbIndex));
}

void RedisConnectionAbstract::getKeys(QString pattern)
{		 
	keysLoadingRunning = true;
	runCommand(QString("keys %1").arg(pattern), -1);	
}

bool RedisConnectionAbstract::isConnected()
{
	return connected;
}

void RedisConnectionAbstract::sendResponse()
{
	executionTimer.stop();	
	commandRunning = false;

	if (keysLoadingRunning) {		
		keysLoadingRunning = false;
		QStringList keys = resp.getValue().toStringList();

		emit keysLoaded(keys);
		return;
	}

	emit responseResived(resp.getValue());
}

Response RedisConnectionAbstract::getLastResponse()
{
	return resp;
}

void RedisConnectionAbstract::executionTimeout()
{
	if (!commandRunning) {
		return;
	}

	return sendResponse();	
}