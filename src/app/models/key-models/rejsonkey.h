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
  void loadRows(
      QVariant, unsigned long,
      std::function<void(const QString&, unsigned long)> callback) override;
  void removeRow(int, ValueEditor::Model::Callback c) override;

 protected:
  void addLoadedRowsToCache(const QVariantList&, QVariant) override {}

 private:
  enum Roles { Value = Qt::UserRole + 1 };
};
