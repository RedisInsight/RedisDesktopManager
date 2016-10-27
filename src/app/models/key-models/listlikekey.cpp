#include "listlikekey.h"

ListLikeKeyModel::ListLikeKeyModel(QSharedPointer<RedisClient::Connection> connection,
                                   QByteArray fullPath, int dbIndex, long long ttl,
                                   QByteArray rowsCountCmd,
                                   QByteArray partialLoadingCmd, QByteArray fullLoadingCmd,
                                   bool fullLoadingCmdSupportsRanges)
    : KeyModel(connection, fullPath, dbIndex, ttl, true,
               rowsCountCmd, partialLoadingCmd, fullLoadingCmd,
               fullLoadingCmdSupportsRanges)
{
}

QStringList ListLikeKeyModel::getColumnNames()
{
    return QStringList() << "row"  << "value";
}

QHash<int, QByteArray> ListLikeKeyModel::getRoles()
{
    QHash<int, QByteArray> roles;
    roles[Roles::Value] = "value";
    roles[Roles::RowNumber] = "row";    
    return roles;
}

QVariant ListLikeKeyModel::getData(int rowIndex, int dataRole)
{
    if (!isRowLoaded(rowIndex))
        return QVariant();

    switch (dataRole) {
        case Value:
            return m_rowsCache[rowIndex];
        case RowNumber:
            return rowIndex+1;
    }

    return QVariant();
}

void ListLikeKeyModel::addLoadedRowsToCache(const QVariantList &rows, int rowStart)
{
    QList<QByteArray> result;

    foreach (QVariant row, rows) {
        result.push_back(row.toByteArray());
    }

    m_rowsCache.addLoadedRange({rowStart, rowStart + result.size() - 1},
                               result);
}
