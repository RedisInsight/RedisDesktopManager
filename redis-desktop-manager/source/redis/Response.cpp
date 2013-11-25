#include "Response.h"
#include "RedisException.h"

Response::Response()
	: responseString(""), lastValidPos(0), itemsCount(0)
{
}

Response::Response(QString & src)
    : responseString(src), lastValidPos(0), itemsCount(0)
{
}

Response::~Response(void)
{
}

void Response::setSource(QString& str)
{
	responseString = str;
}

void Response::clear()
{
	responseString.clear();
	lastValidPos = 0;
	itemsCount = 0;
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

QVariant Response::parseBulk(const QString& response)
{
	int endOfFirstLine = response.indexOf("\r\n");
	int responseSize = getSizeOfBulkReply(response, endOfFirstLine);	

	if (responseSize != -1) {
		return QVariant(response.mid(endOfFirstLine + 2, responseSize));		
	}

	return QVariant();
}

QStringList Response::parseMultiBulk(const QString& response)
{	
	int endOfFirstLine = response.indexOf("\r\n");
	int responseSize = getSizeOfBulkReply(response, endOfFirstLine);			

	if (responseSize == 0) 
	{	
		return QStringList();
	}

	QStringList parsedResult; ResponseType type; int firstItemLen, firstPosOfEndl, bulkLen;

	parsedResult.reserve(responseSize+5);

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
	return isReplyValid(responseString);
}

bool Response::isReplyValid(const QString & responseString)
{
	if (responseString.isEmpty()) 
	{
		return false;
	}

	ResponseType type = getResponseType(responseString);

	switch (type)
	{
		case Status:
		case Error:		
		case Unknown:
		default:
			return isReplyGeneralyValid(responseString);				

		case Integer:
			return isReplyGeneralyValid(responseString) 
				&& isIntReplyValid(responseString);

		case Bulk:  
			return isReplyGeneralyValid(responseString) 
				&& isBulkReplyValid(responseString);		

		case MultiBulk:
			return isReplyGeneralyValid(responseString) 
				&& isMultiBulkReplyValid(responseString);	
	}	
}

bool Response::isReplyGeneralyValid(const QString& r)
{
	return r.endsWith("\r\n");
}

int Response::getPosOfNextItem(const QString &r, int startPos = 0)
{
	if (startPos >= r.size()) {
		return -1;
	}

	ResponseType type = getResponseType(r.at(startPos));

	int endOfFirstLine = r.indexOf("\r\n", startPos);

	int responseSize;

	switch (type)
	{	
	case Integer:
		return endOfFirstLine+2;

	case Bulk:  		
		responseSize = getSizeOfBulkReply(r, endOfFirstLine, startPos);

		if (responseSize == -1) {
			return endOfFirstLine+2;
		} else {
			return endOfFirstLine+responseSize+4;
		}
		break;
	default:
		return -1;
	}

}

bool Response::isIntReplyValid(const QString& r)
{
    return !r.isEmpty();
}

bool Response::isBulkReplyValid(const QString& r)
{			
	int endOfFirstLine = r.indexOf("\r\n");
	int responseSize = getSizeOfBulkReply(r, endOfFirstLine);

	if (responseSize == -1) {
		return true;
	}

	int actualSizeOfResponse = r.size() - endOfFirstLine - 4;

	// we need not strict check for using this method for validation multi-bulk items
	if (actualSizeOfResponse < responseSize) {
		return false;
	}

	return true;
}

bool Response::isMultiBulkReplyValid(const QString& r) 
{	
	int endOfFirstLine = r.indexOf("\r\n");
	int responseSize = getSizeOfBulkReply(r, endOfFirstLine);

	if (responseSize <= 0) {
		return true;
	}
		
	//fast validation based on string size
	int minimalReplySize = responseSize * 4 + endOfFirstLine; // 4 is [type char] + [digit char] + [\r\n]

	int responseStringSize = r.size();

	if (responseStringSize < minimalReplySize) {
		return false;
	}

	//detailed validation
	int currPos = (lastValidPos > 0) ? lastValidPos : endOfFirstLine + 2;
	int lastPos = 0;

	do {

		currPos = getPosOfNextItem(r, currPos);

		if (currPos != -1) {
			lastPos = currPos;
		}

		if (currPos != -1 && currPos != responseStringSize) {
			lastValidPos = currPos;
		}

	} while (currPos != -1 && ++itemsCount);


	if (itemsCount < responseSize || (lastPos != responseStringSize)) {
		return false;
	}

	return true;	
}

int Response::getSizeOfBulkReply(const QString& reply, int endOfFirstLine, int beginFrom) 
{
	if (endOfFirstLine == -1) {
		endOfFirstLine = reply.indexOf("\r\n", beginFrom);
	}

	QString strRepresentaton;
	
	for (int pos = beginFrom + 1; pos < endOfFirstLine; pos++) {
		strRepresentaton += reply.at(pos);
	}

	return strRepresentaton.toInt();		
}

QString Response::valueToString(QVariant& value)
{
	if (value.isNull()) 
	{
		return "NULL";
	} else if (value.type() == QVariant::StringList) {
		return value.toStringList().join("\r\n");
	} 

	return value.toString();
}

int Response::getLoadedItemsCount()
{
	return itemsCount;
}
