#pragma once
#include <QtCore>
#include "exception.h"

namespace RedisClient {
class Response
{    
    ADD_EXCEPTION

public:
    enum Type { Status, Error, Integer, Bulk, MultiBulk, Unknown };

public:
    Response();
    Response(const QByteArray &);
    virtual ~Response(void);

    QVariant getValue();        
    Type getType();
    int getLoadedItemsCount();
    QByteArray source();
    QString toString();

    bool isErrorMessage() const;
    bool isDisabledCommandErrorMessage() const;
    bool isOkMessage() const;
    bool isValid();

public:
    void setSource(const QByteArray&);
    void appendToSource(QString&);
    void appendToSource(QByteArray&);
    void clear();

    static QString valueToHumanReadString(QVariant&);

protected:
    Type getResponseType(const QByteArray&) const;
    Type getResponseType(const char) const;

    //parsers
    QByteArray parseBulk(const QByteArray& response);
    QVariantList parseMultiBulk(const QByteArray& response);
    QByteArray getStringResponse(const QByteArray& response);

    int getSizeOfBulkReply(const QByteArray& reply, int endOfFirstLine = -1, int beginFrom = 0);    

    //validations
    bool isReplyValid(const QByteArray&);

    /** checks general validation rules **/
    bool isReplyGeneralyValid(const QByteArray& );
    bool isIntReplyValid(const QByteArray&);
    bool isBulkReplyValid(const QByteArray&);
    bool isMultiBulkReplyValid(const QByteArray&);    

    int getPosOfNextItem(const QByteArray &, int);

protected:
    QByteArray m_responseSource;

    //cache previous validation markers
    int m_lastValidPos;
    int m_itemsCount;
};
}
