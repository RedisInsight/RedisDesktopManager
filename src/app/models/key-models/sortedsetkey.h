#pragma once
#include "abstractkey.h"

class SortedSetKeyModel : public KeyModel<QPair<QByteArray, QByteArray>> {
 public:
  SortedSetKeyModel(QSharedPointer<RedisClient::Connection> connection,
                    QByteArray fullPath, int dbIndex, long long ttl);

  QString type() override;
  QStringList getColumnNames() override;
  QHash<int, QByteArray> getRoles() override;
  QVariant getData(int rowIndex, int dataRole) override;

  void addRow(const QVariantMap&, Callback c) override;
  virtual void updateRow(int rowIndex, const QVariantMap&, Callback c) override;
  void removeRow(int, Callback c) override;

 protected:
  int addLoadedRowsToCache(const QVariantList& list,
                           QVariant rowStart) override;

 private:
  enum Roles { RowNumber = Qt::UserRole + 1, Value, Score };

  void addSortedSetRow(const QByteArray& value, QByteArray score, Callback c,
                       bool updateExisting = false);
  void deleteSortedSetRow(const QByteArray& value, Callback c);
};
