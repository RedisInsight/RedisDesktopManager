#pragma once
#include "abstractkey.h"

struct TairHashValue {
    QByteArray value;
    qlonglong ttl;
    qlonglong version;
};

class TairHashKeyModel : public KeyModel<QPair<QByteArray, TairHashValue>> {
 public:
  TairHashKeyModel(QSharedPointer<RedisClient::Connection> connection,
               QByteArray fullPath, int dbIndex, long long ttl);

  QString type() override;
  QStringList getColumnNames() override;
  QHash<int, QByteArray> getRoles() override;

  QVariant getData(int rowIndex, int dataRole) override;
  void addRow(const QVariantMap &, Callback) override;
  virtual void updateRow(int rowIndex, const QVariantMap &, Callback) override;
  void removeRow(int, Callback) override;

 protected:
  int addLoadedRowsToCache(const QVariantList &list,
                           QVariant rowStart) override;

 private:
  enum Roles { RowNumber = Qt::UserRole + 1, Key, Value, TTL, Version };

  void setHashRow(const QByteArray &hashKey, const QByteArray &hashValue,
                  Callback c, bool updateIfNotExist = true);
  void deleteHashRow(const QByteArray &hashKey, Callback c);

  void loadMetadata(const QByteArray& hashKey, int row);
};
