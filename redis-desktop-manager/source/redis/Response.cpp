#include "Response.h"
#include "RedisException.h"

Response::Response()
	: responseString("")
{
}


Response::~Response(void)
{
}

void Response::setSource(QString& str)
{
	responseString = str;
}

QString Response::source()
{
	return responseString;
}

void Response::appendToSource(QString& src)
{
	responseString.append(src);
}

void Response::appendToSource(QByteArray& src)
{
	responseString.append(src);
}

QVariant Response::getValue()
{
	if (responseString.isEmpty()) {
		return QVariant();
	}

	ResponseType t = getResponseType(responseString);

	QVariant  parsedResponse;

	switch (t) {

	case Status:
	case Error:		
		parsedResponse = QVariant(getStringResponse(responseString));
		break;

	case Integer:		
		parsedResponse = QVariant(getStringResponse(responseString).toInt());
		break;

	case Bulk:
		parsedResponse = parseBulk(responseString);
		break;

	case MultiBulk: 
		parsedResponse = QVariant(parseMultiBulk(responseString));		
		break;
	case Unknown:
		break;
	}

	return parsedResponse;
}	

QVariant Response::parseBulk(QString response)
{
	int endOfFirstLine = response.indexOf("\r\n");
	int responseSize=response.mid(1, endOfFirstLine-1).toInt();

	if (responseSize != -1) {
		return QVariant(response.mid(endOfFirstLine + 2, responseSize));		
	}

	return QVariant();
}

QStringList Response::parseMultiBulk(QString response)
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

Response::ResponseType Response::getResponseType(QString r) 
{	
	return getResponseType(r.at(0));
}

Response::ResponseType Response::getResponseType(const QChar typeChar) 
{	
	if (typeChar == '+') return Status; 
	if (typeChar == '-') return Error;
	if (typeChar == ':') return Integer;
	if (typeChar == '$') return Bulk;
	if (typeChar == '*') return MultiBulk;

	return Unknown;
}

QString Response::getStringResponse(QString response)
{
	return 	response.mid(1, response.length() - 3);
}

bool Response::isValid()
{

	if (responseString.isEmpty())
		return false;

	ResponseType type = getResponseType(responseString);

	switch (type)
	{
	case Status:
	case Error:
	case Integer:				
		return isIntReplyValid(responseString);				

	case Bulk:  
		return isBulkReplyValid(responseString);		

	case MultiBulk:
		return isMultiBulkReplyValid(responseString);

	default: // ignore info responses
		return true;			
	}		
}

bool Response::isIntReplyValid(QString r)
{
	return r.endsWith("\r\n");
}

bool Response::isBulkReplyValid(QString r)
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

int Response::getSizeOfBulkReply(QString mb) 
{
	return mb.mid(1).toInt();		
}

bool Response::isMultiBulkReplyValid(QString r) 
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
		case Response::Integer:

			if (!isIntReplyValid(QString("%1\r\n").arg(item))) return false;

			itemsCount++;

			break;

		case Response::Bulk:

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
