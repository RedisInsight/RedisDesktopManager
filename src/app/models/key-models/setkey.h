#pragma once
#include "listlikekey.h"

class SetKeyModel : public ListLikeKeyModel {
 public:
  SetKeyModel(QSharedPointer<RedisClient::Connection> connection,
              QByteArray fullPath, int dbIndex, long long ttl);

  QString type() override;

  void addRow(const QVariantMap &, Callback c) override;
  virtual void updateRow(int rowIndex, const QVariantMap &,
                         Callback c) override;
  void removeRow(int, Callback c) override;

 private:
  void addSetRow(const QByteArray &value, Callback c);
  void deleteSetRow(const QByteArray &value, Callback c);
};
