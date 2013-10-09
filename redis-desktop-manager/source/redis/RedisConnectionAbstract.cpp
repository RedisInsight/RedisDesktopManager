#include "RedisConnectionAbstract.h"

RedisConnectionAbstract::RedisDatabases RedisConnectionAbstract::getDatabases()
{
	RedisDatabases availableDatabeses;

	QVariant rawDbCount = execute("config get databases");

	QStringList dbInfo = rawDbCount.toStringList();

	if (rawDbCount.isNull() || dbInfo.size() != 2) 
		return availableDatabeses;
	
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
		parsedResponse = QVariant(getStringResponse(response));
		break;

	case Integer:		
		parsedResponse = QVariant(getStringResponse(response).toInt());
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
		parsedResponse = QVariant(parseMultiBulk(response));		
		break;
	}


	return parsedResponse;
}	

QStringList RedisConnectionAbstract::parseMultiBulk(QString response)
{	
	int endOfFirstLine = response.indexOf("\r\n");
	int responseSize=response.mid(1, endOfFirstLine-1).toInt();			

	if (responseSize == 0) 
	{	
		return QStringList();
	}

	QStringList parsedResult; QString item; ResponseType type; int firstItemLen, firstPosOfEndl, bulkLen;

	for (int currPos = endOfFirstLine + 2, respStringSize = response.size(); currPos < respStringSize;) 
	{
		type = getResponseType(response.at(currPos));

		firstPosOfEndl = response.indexOf("\r\n", currPos);
		firstItemLen = firstPosOfEndl - currPos-1;

		if (type == Integer) 
		{											
			parsedResult << response.mid(currPos+1, firstItemLen);

			currPos = firstPosOfEndl + 2;
			continue;
		} 

		if (type == Bulk) 
		{						
			bulkLen = response.mid(currPos+1, firstItemLen).toInt();

			if (bulkLen == 0) 
			{
				parsedResult << "";
				currPos = firstPosOfEndl + 4;
			} else {
				parsedResult << response.mid(firstPosOfEndl+2, bulkLen);
				currPos = firstPosOfEndl + bulkLen + 4;
			}

			continue;
		} 

		if (type == MultiBulk) 
		{
			throw RedisException("Recursive parsing of MultiBulk replies not supported");
		}
	}			

	return parsedResult;
}

RedisConnectionAbstract::ResponseType RedisConnectionAbstract::getResponseType(QString r) 
{	
	return getResponseType(r.at(0));
}

RedisConnectionAbstract::ResponseType RedisConnectionAbstract::getResponseType(const QChar typeChar) 
{	
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