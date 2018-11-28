#pragma once
#include "abstractkey.h"

class ReJSONKeyModel : public KeyModel<QByteArray> {
 public:
  ReJSONKeyModel(QSharedPointer<RedisClient::Connection> connection,
                 QByteArray fullPath, int dbIndex, long long ttl);

  QString type() override;
  QStringList getColumnNames() override;
  QHash<int, QByteArray> getRoles() override;
  QVariant getData(int rowIndex, int dataRole) override;

  void addRow(const QVariantMap&, ValueEditor::Model::Callback c) override;
  virtual void updateRow(int rowIndex, const QVariantMap& row,
                         ValueEditor::Model::Callback c) override;
  void loadRows(QVariant, unsigned long, LoadRowsCallback callback) override;
  void removeRow(int, ValueEditor::Model::Callback c) override;

 protected:
  int addLoadedRowsToCache(const QVariantList&, QVariant) override { return 1; }

 private:
  enum Roles { Value = Qt::UserRole + 1 };
};
