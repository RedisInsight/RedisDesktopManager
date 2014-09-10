#ifndef HASHKEYMODEL_H
#define HASHKEYMODEL_H

#include "abstractkey.h"

class HashKeyModel : public KeyModel
{
    Q_OBJECT

public:
    HashKeyModel(RedisClient::Connection * db, const QString &keyName, int dbIndex);

    void setCurrentPage(int);

    int itemsCount();

    void loadValue();

    void updateValue(const QString& value, const QModelIndex *cellIndex);


protected slots:
    void loadedUpdateStatus(RedisClient::Response);
};

#endif // HASHKEYMODEL_H
