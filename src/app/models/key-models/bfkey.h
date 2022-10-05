#pragma once
#include "stringkey.h"

class BloomFilterKeyModel : public KeyModel<QVariant> {
 public:
  BloomFilterKeyModel(QSharedPointer<RedisClient::Connection> connection,
                      QByteArray fullPath, int dbIndex, long long ttl, QString filterFamily="bf");

  QString type() override;
  QStringList getColumnNames() override;
  QHash<int, QByteArray> getRoles() override;
  QVariant getData(int rowIndex, int dataRole) override;

  void addRow(const QVariantMap&, Callback c) override;

  virtual void updateRow(int, const QVariantMap&,
                         Callback) override {
      // NOTE(u_glide): BF/CF is read-only
  }
  void loadRows(QVariant, unsigned long, LoadRowsCallback callback) override;

  void removeRow(int, Callback) override {
      // NOTE(u_glide): BF/CF is read-only
  }

  virtual unsigned long rowsCount() override { return m_rowCount; }

 protected:
  int addLoadedRowsToCache(const QVariantList&, QVariant) override { return 1; }

 private:
  enum Roles { Value = Qt::UserRole + 1 };
  QString m_type;
};
