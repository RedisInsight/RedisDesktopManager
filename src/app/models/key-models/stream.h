#pragma once
#include "abstractkey.h"

class StreamKeyModel : public KeyModel<QPair<QByteArray, QVariant>> {
 public:
  StreamKeyModel(QSharedPointer<RedisClient::Connection> connection,
                 QByteArray fullPath, int dbIndex, long long ttl);

  QString type() override;
  QStringList getColumnNames() override;
  QHash<int, QByteArray> getRoles() override;

  QVariant getData(int rowIndex, int dataRole) override;

  void addRow(const QVariantMap &, Callback) override;

  virtual void updateRow(int rowIndex, const QVariantMap &, Callback) override;

  void removeRow(int, Callback) override;

   void loadRowsCount(ValueEditor::Model::Callback c) override;

 protected:
  int addLoadedRowsToCache(const QVariantList &list,
                           QVariant rowStart) override;
  virtual QList<QByteArray> getRangeCmd(QVariant rowStartId,
                                        unsigned long count) override;

 protected:
  enum Roles { RowNumber = Qt::UserRole + 1, ID, Value };
};
