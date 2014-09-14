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
    ~Response(void);

    QVariant getValue();
    bool isValid();    

    void setSource(const QByteArray&);
    QByteArray source();

    void clear();

    void appendToSource(QString&);
    void appendToSource(QByteArray&);

    static QString valueToHumanReadString(QVariant&);

    int getLoadedItemsCount();

    bool isErrorMessage() const;
    bool isOkMessage() const;

    QString toString();

    enum Type
    {
        Status, Error, Integer, Bulk, MultiBulk, Unknown
    };

    Type getType();

private:
    QByteArray responseSource;

    //cache previous validation markers
    int lastValidPos;
    int itemsCount;

    Type getResponseType(const QByteArray&) const;
    Type getResponseType(const char) const;

    //parsers
    QString parseBulk(const QByteArray& response);
    QStringList parseMultiBulk(const QByteArray& response);
    QString getStringResponse(const QByteArray& response);

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
