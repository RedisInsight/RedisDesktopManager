#pragma once

#include <QtCore>
#include "exception.h"

namespace RedisClient {
class Response
{    
    ADD_EXCEPTION

public:
    Response();
    Response(const QByteArray &);
    virtual ~Response(void);

    QVariant getValue();        

    void setSource(const QByteArray&);
    QByteArray source();

    void clear();

    void appendToSource(QString&);
    void appendToSource(QByteArray&);

    static QString valueToHumanReadString(QVariant&);

    int getLoadedItemsCount();

    bool isErrorMessage() const;
    bool isOkMessage() const;   
    bool isValid();

    QString toString();

    enum Type
    {
        Status, Error, Integer, Bulk, MultiBulk, Unknown
    };

    Type getType();

protected:
    QByteArray responseSource;

    //cache previous validation markers
    int lastValidPos;
    int itemsCount;

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
};
}
