#include "response.h"

RedisClient::Response::Response()
    : m_responseSource(""), m_lastValidPos(0), m_itemsCount(0)
{
}

RedisClient::Response::Response(const QByteArray & src)
    : m_responseSource(src), m_lastValidPos(0), m_itemsCount(0)
{
}

RedisClient::Response::~Response(void)
{
}

void RedisClient::Response::setSource(const QByteArray& src)
{
    m_responseSource = src;
}

void RedisClient::Response::clear()
{
    m_responseSource.clear();
    m_lastValidPos = 0;
    m_itemsCount = 0;
}

QByteArray RedisClient::Response::source()
{
    return m_responseSource;
}

void RedisClient::Response::appendToSource(QString& src)
{
    m_responseSource.append(src);
}

void RedisClient::Response::appendToSource(QByteArray& src)
{
    m_responseSource.append(src);
}

QString RedisClient::Response::toString()
{
    return m_responseSource.left(1500);
}

RedisClient::Response::Type RedisClient::Response::getType()
{
    return getResponseType(m_responseSource);
}

QVariant RedisClient::Response::getValue()
{
    if (m_responseSource.isEmpty()) {
        return QVariant();
    }

    Type t = getResponseType(m_responseSource);

    QVariant  parsedResponse;

    try {
        switch (t) {
            case Status:
            case Error:
                parsedResponse = QVariant(getStringResponse(m_responseSource));
                break;

            case Integer:                
                parsedResponse = QVariant(getStringResponse(m_responseSource).toLongLong());
                break;

            case Bulk:
                parsedResponse = QVariant(parseBulk(m_responseSource));
                break;

            case MultiBulk:
                parsedResponse = QVariant(parseMultiBulk(m_responseSource));
                break;
            case Unknown:
                break;
        }
    } catch (Response::Exception &e) {
        parsedResponse = QVariant(QStringList() << e.what());
    }

    return parsedResponse;
}    

QByteArray RedisClient::Response::parseBulk(const QByteArray& response)
{
    int endOfFirstLine = response.indexOf("\r\n");
    int responseSize = getSizeOfBulkReply(response, endOfFirstLine);    

    if (responseSize != -1) {
        return response.mid(endOfFirstLine + 2, responseSize);        
    }

    return QByteArray();
}

QVariantList RedisClient::Response::parseMultiBulk(const QByteArray& response)
{    
    int endOfFirstLine = response.indexOf("\r\n");
    int responseSize = getSizeOfBulkReply(response, endOfFirstLine);            

    if (responseSize == 0)     
        return QVariantList();

    QVariantList parsedResult;
    Type type;
    int firstItemLen, firstPosOfEndl, bulkLen;

    parsedResult.reserve(responseSize+5);

    for (int currPos = endOfFirstLine + 2, respStringSize = response.size(); currPos < respStringSize;) 
    {        
        type = getResponseType(response.at(currPos));

        firstPosOfEndl = response.indexOf("\r\n", currPos);
        firstItemLen = firstPosOfEndl - currPos-1;

        if (type == Integer
                || type == Status
                || type == Error) {
            parsedResult << QVariant(response.mid(currPos+1, firstItemLen));

            currPos = firstPosOfEndl + 2;
            continue;
        } else if (type == Bulk) {                                    
            bulkLen = response.mid(currPos+1, firstItemLen).toInt();

            if (bulkLen == 0) 
            {
                parsedResult << QVariant("");
                currPos = firstPosOfEndl + 4;
            } else {
                parsedResult << QVariant(response.mid(firstPosOfEndl+2, bulkLen));
                currPos = firstPosOfEndl + bulkLen + 4;
            }

            continue;
        } else if (type == MultiBulk) {
            int posOfNextItemAfterArray = getPosOfNextItem(response, currPos);

            QVariantList result = parseMultiBulk(response.mid(currPos, posOfNextItemAfterArray - currPos));
            parsedResult << QVariant(result);

            currPos = posOfNextItemAfterArray;
        } else {
            break;
        }
    }            

    return parsedResult;
}

RedisClient::Response::Type RedisClient::Response::getResponseType(const QByteArray & r) const
{    
    const char typeChar = (r.length() == 0)? ' ' : r.at(0);
    return getResponseType(typeChar);
}

RedisClient::Response::Type RedisClient::Response::getResponseType(const char typeChar) const
{    
    if (typeChar == '+') return Status; 
    if (typeChar == '-') return Error;
    if (typeChar == ':') return Integer;
    if (typeChar == '$') return Bulk;
    if (typeChar == '*') return MultiBulk;
    return Unknown;
}

QByteArray RedisClient::Response::getStringResponse(const QByteArray& response)
{
    return response.mid(1, response.length() - 3);
}

bool RedisClient::Response::isValid()
{
    return isReplyValid(m_responseSource);
}

bool RedisClient::Response::isReplyValid(const QByteArray & responseString)
{
    if (responseString.isEmpty())     
        return false;    

    Type type = getResponseType(responseString);

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

bool RedisClient::Response::isReplyGeneralyValid(const QByteArray& r)
{
    return r.endsWith("\r\n");
}

int RedisClient::Response::getPosOfNextItem(const QByteArray &r, int startPos = 0)
{
    if (startPos >= r.size()) {
        return -1;
    }

    Type type = getResponseType(r.at(startPos));
    int endOfFirstLine = r.indexOf("\r\n", startPos);
    int responseSize;

    switch (type)
    {    
    case Status:
    case Error:
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
    case MultiBulk:
        responseSize = getSizeOfBulkReply(r, endOfFirstLine, startPos);

        if (responseSize == -1 || responseSize == 0) {
            return endOfFirstLine+2;
        } else {
            int lastPos = endOfFirstLine+2;
            for (int currentIndex = 0; currentIndex < responseSize; currentIndex++) {
                lastPos = getPosOfNextItem(r, lastPos);

                if (lastPos == -1)
                    return lastPos;
            }
            return lastPos;
        }
        break;

    default:
        return -1;
    }
}

bool RedisClient::Response::isIntReplyValid(const QByteArray& r)
{
    return !r.isEmpty();
}

bool RedisClient::Response::isBulkReplyValid(const QByteArray& r)
{            
    int endOfFirstLine = r.indexOf("\r\n");
    int responseSize = getSizeOfBulkReply(r, endOfFirstLine);

    if (responseSize == -1) {
        return true;
    }

    int actualSizeOfResponse = r.size() - endOfFirstLine - 4;

    // we need not strict condition for using this method for validation multi-bulk items
    if (actualSizeOfResponse < responseSize) {
        return false;
    }

    return true;
}

bool RedisClient::Response::isMultiBulkReplyValid(const QByteArray& r)
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
    int currPos = (m_lastValidPos > 0) ? m_lastValidPos : endOfFirstLine + 2;
    int lastPos = 0;

    do {
        currPos = getPosOfNextItem(r, currPos);

        if (currPos != -1) {
            lastPos = currPos;
        }

        if (currPos != -1 && currPos != responseStringSize) {
            m_lastValidPos = currPos;
        }

    } while (currPos != -1 && ++m_itemsCount);

    if (m_itemsCount < responseSize || (lastPos != responseStringSize)) {
        return false;
    }

    return true;    
}

int RedisClient::Response::getSizeOfBulkReply(const QByteArray& reply, int endOfFirstLine, int beginFrom)
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

// TBD: add pretty printing
QString RedisClient::Response::valueToHumanReadString(QVariant& value)
{
    if (value.isNull()) 
    {
        return "NULL";
    } else if (value.type() == QVariant::StringList) {
        return value.toStringList().join("\r\n");
    } else if (value.type() == QVariant::Type::List) {
        QVariantList val = value.toList();
        QString result;
        for (int i = 0; i < val.size(); i++) {
            result.append(QString("%1) ").arg(QString::number(i+1)));
            if (val.at(i).type() == QVariant::Type::List) {
                result.append(val.at(i).toStringList().join("\r\n"));
            } else {
                result.append(val.at(i).toString());
            }
            result.append("\n");
        }
        return result;
    }

    return value.toString();
}

int RedisClient::Response::getLoadedItemsCount()
{
    return m_itemsCount;
}

bool RedisClient::Response::isErrorMessage() const
{
    return getResponseType(m_responseSource) == Error
            && m_responseSource.startsWith("-ERR");
}

bool RedisClient::Response::isDisabledCommandErrorMessage() const
{
    return isErrorMessage() && m_responseSource.contains("unknown command");
}

bool RedisClient::Response::isOkMessage() const
{
    return getResponseType(m_responseSource) == Status
            && m_responseSource.startsWith("+OK");
}
