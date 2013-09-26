#include "RedisConnectionAbstract.h"

RedisConnectionAbstract::RedisDatabases RedisConnectionAbstract::getDatabases()
{
	RedisDatabases availableDatabeses;

	QVariant rawDbCount = execute("config get databases");

	if (rawDbCount.isNull()) 
		return availableDatabeses;

	QStringList dbInfo = rawDbCount.toStringList();
	int dbCount = dbInfo.at(1).toInt();

	// build db list
	for (int dbIndex = 0; dbIndex < dbCount; ++dbIndex)
	{
		availableDatabeses.insert(QString("db%1").arg(dbIndex), 0);
	}

	//	Get keys count		
	QVariant result = execute("INFO keyspace");

	if (result.isNull()) 
		return availableDatabeses;

	QStringList keyspaceInfo = result.toString().split("\r\n", QString::SkipEmptyParts);

	QRegExp getDbAndKeysCount("(db\\d):keys=(\\d+),expires=(\\d+)");

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

	return availableDatabeses;
} 

void RedisConnectionAbstract::selectDb(int dbIndex)
{
	execute(QString("select %1").arg(dbIndex));
}

QStringList RedisConnectionAbstract::getKeys(QString pattern)
{		 
	QVariant rawKeys = execute(QString("keys %1").arg(pattern));

	if (rawKeys.isNull()) return QStringList();

	return rawKeys.toStringList();
}

bool RedisConnectionAbstract::isConnected()
{
	return connected;
}

QString RedisConnectionAbstract::prepareCommand(QString command)
{
	QStringList parts = command.split(" ", QString::SkipEmptyParts, Qt::CaseInsensitive);

	QString formattedCmd("*");
	formattedCmd.append(QString("%1\r\n").arg(parts.length()));

	for (QString part : parts) {
		formattedCmd.append(QString("$%1\r\n%2\r\n")
			.arg(QString("%1").arg(part.length()), part));
	}

	return formattedCmd;
}

QVariant RedisConnectionAbstract::parseResponse(QString response)
{
	if (response.isEmpty()) {
		return QVariant();
	}

	ResponseType t = getResponseType(response);

	QVariant  parsedResponse;

	switch (t) {

	case Status:
	case Error:
		response = getStringResponse(response);
		parsedResponse = QVariant(response);
		break;

	case Integer:
		response = getStringResponse(response);
		parsedResponse = QVariant(response.toInt());
		break;

	case Bulk:
		{
			int endOfFirstLine = response.indexOf("\r\n");
			int responseSize=response.mid(1, endOfFirstLine-1).toInt();

			if (responseSize != -1) {
				response = response.mid(endOfFirstLine + 2, responseSize);
				parsedResponse = QVariant(response);		
			}
		}
		break;

	case MultiBulk:
		{
			QStringList parts = response.split("\r\n", QString::SkipEmptyParts, Qt::CaseInsensitive);
			int responseSize = getSizeOfBulkReply(parts.at(0));

			if (responseSize > 0) 
			{						
				QStringList parsedResult;

				// skip first string with length
				for (int i=1; i < parts.size(); ++i) 
				{
					QString item = parts.at(i);											

					if (getResponseType(item) == Integer) {
						parsedResult << item.mid(1);
					} else if (getResponseType(item) == Bulk) {
						continue;
					} else {
						parsedResult << item;
					}
				}

				parsedResponse = QVariant(parsedResult);

			} else if (responseSize == 0) { //empty multi-bulk reply
				parsedResponse = QVariant(QStringList());
			}
		}
		break;
	}


	return parsedResponse;
}	

RedisConnectionAbstract::ResponseType RedisConnectionAbstract::getResponseType(QString r) 
{
	QChar typeChar = r.at(0);

	if (typeChar == '+') return Status; 
	if (typeChar == '-') return Error;
	if (typeChar == ':') return Integer;
	if (typeChar == '$') return Bulk;
	if (typeChar == '*') return MultiBulk;

	return Unknown;
}

QString RedisConnectionAbstract::getStringResponse(QString response)
{
	return 	response.mid(1, response.length() - 3);
}

bool RedisConnectionAbstract::isFullResponseRecieved(QString r)
{
	if (r.isEmpty()) return false;

	ResponseType type = getResponseType(r);

	switch (type)
	{
	case Status:
	case Error:
	case Integer:				
		return isIntReplyValid(r);				

	case Bulk:  
		return isBulkReplyValid(r);		

	case MultiBulk:
		return isMultiBulkReplyValid(r);

	default: // ignore info responses
		return true;			
	}		
}

bool RedisConnectionAbstract::isIntReplyValid(QString r)
{
	return r.endsWith("\r\n");
}

bool RedisConnectionAbstract::isBulkReplyValid(QString r)
{
	if (!r.endsWith("\r\n"))
		return false;				

	int endOfFirstLine = r.indexOf("\r\n");
	int responseSize=r.mid(1, endOfFirstLine-1).toInt();

	if (responseSize == -1) {
		return true;
	}

	int actualSizeOfResponse = r.mid(endOfFirstLine+2).size() - 2;

	if (actualSizeOfResponse != responseSize) {
		return false;
	}

	return true;
}

int RedisConnectionAbstract::getSizeOfBulkReply(QString mb) 
{
	return mb.mid(1).toInt();		
}

bool RedisConnectionAbstract::isMultiBulkReplyValid(QString r) 
{
	if (!r.endsWith("\r\n"))
		return false;

	QStringList parts = r.split("\r\n", QString::SkipEmptyParts, Qt::CaseInsensitive);
	int responseSize = getSizeOfBulkReply(parts.at(0));

	if (responseSize <= 0) {
		return true;
	}

	// count items count
	// skip first string with length
	int itemsCount = 0;
	for (int i=1; i < parts.size(); ++i) 
	{
		QString item = parts.at(i);											
		ResponseType type = getResponseType(item);

		switch (type)
		{			
		case RedisConnectionAbstract::Integer:

			if (!isIntReplyValid(QString("%1\r\n").arg(item))) return false;

			itemsCount++;

			break;

		case RedisConnectionAbstract::Bulk:

			if (getSizeOfBulkReply(item) <= 0) {
				itemsCount++;
			} else {

				int leftLines = parts.size() - 1 - i;

				if (leftLines == 0) return false;

				QString bulk = QString("%1\r\n%2\r\n").arg(item).arg(parts.at(i+1));						

				if (!isBulkReplyValid(bulk)) {
					return false;
				} else {
					itemsCount++;
					i++;
				}
			}

			break;

		default:
			return false;
		}
	}

	if (itemsCount != responseSize) {
		return false;
	}

	return true;	
}

bool RedisConnectionAbstract::waitForData(int ms)
{
	//wait for data
	QEventLoop loop;
	QTimer timeoutTimer;

	//configure sync objects
	timeoutTimer.setSingleShot(true);
	QObject::connect(&timeoutTimer, SIGNAL(timeout()), &loop, SLOT(quit()));

	timeoutTimer.start(ms); 
	loop.exec();

	return true;
}