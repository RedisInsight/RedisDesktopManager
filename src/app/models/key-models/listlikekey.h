#pragma once
#include "abstractkey.h"

class ListLikeKeyModel : public KeyModel<QByteArray> {
 public:
  ListLikeKeyModel(QSharedPointer<RedisClient::Connection> connection,
                   QByteArray fullPath, int dbIndex, long long ttl,
                   QByteArray rowsCountCmd, QByteArray rowsLoadCmd);

  QStringList getColumnNames() override;
  QHash<int, QByteArray> getRoles() override;
  QVariant getData(int rowIndex, int dataRole) override;

 protected:
  enum Roles { RowNumber = Qt::UserRole + 1, Value };

 protected:
  int addLoadedRowsToCache(const QVariantList& rows,
                           QVariant rowStart) override;
};
