#pragma once

#include <QtCore>

class TestResponse;

namespace RedisClient {
class Response
{
    friend class TestResponse;

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

private:

    QByteArray responseSource;

    //type
    enum ResponseType 
    {
        Status, Error, Integer, Bulk, MultiBulk, Unknown            
    };

    //cache previous validation markers
    int lastValidPos;
    int itemsCount;

    ResponseType getResponseType(const QByteArray&) const;    
    ResponseType getResponseType(const char) const;

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
