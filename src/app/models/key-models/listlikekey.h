#pragma once
#include "abstractkey.h"

class ListLikeKeyModel : public KeyModel<QByteArray>
{
public:
    ListLikeKeyModel(QSharedPointer<RedisClient::Connection> connection,
                QString fullPath, int dbIndex, int ttl,
                     QString rowsCountCmd,
                     QString partialLoadingCmd,
                     QString fullLoadingCmd,
                     bool fullLoadingCmdSupportsRanges);


    QStringList getColumnNames() override;
    QHash<int, QByteArray> getRoles() override;
    QVariant getData(int rowIndex, int dataRole) override;

protected:
    enum Roles { Value = Qt::UserRole + 1, BinaryValue, RowNumber};

protected:
    void addLoadedRowsToCache(const QVariantList& rows, int rowStart) override;
};
