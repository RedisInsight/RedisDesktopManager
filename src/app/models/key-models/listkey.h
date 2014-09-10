#ifndef LISTKEYMODEL_H
#define LISTKEYMODEL_H

#include "abstractkey.h"

class ListKeyModel : public KeyModel
{
    Q_OBJECT

public:
    ListKeyModel(RedisClient::Connection * db, const QString &keyName, int dbIndex);

    void setCurrentPage(int page);

    void loadValue();

    void updateValue(const QString& value, const QModelIndex *cellIndex);

protected slots:
    void loadedUpdateStatus(RedisClient::Response);
};

#endif // LISTKEYMODEL_H
