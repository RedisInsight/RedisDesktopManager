#pragma once
#include "abstractkey.h"

class StringKeyModel : public KeyModel<QByteArray> {
 public:
  StringKeyModel(QSharedPointer<RedisClient::Connection> connection,
                 QByteArray fullPath, int dbIndex, long long ttl);

  QString type() override;
  QStringList getColumnNames() override;
  QHash<int, QByteArray> getRoles() override;
  QVariant getData(int rowIndex, int dataRole) override;

  void addRow(const QVariantMap&, Callback) override;
  virtual void updateRow(int rowIndex, const QVariantMap& row, Callback) override;
  void loadRows(QVariant, unsigned long,
                std::function<void(const QString&, unsigned long)> callback) override;
  void removeRow(int, Callback) override;

 protected:
  void addLoadedRowsToCache(const QVariantList&, QVariant) override {}

 private:
  enum Roles { Value = Qt::UserRole + 1 };
};
