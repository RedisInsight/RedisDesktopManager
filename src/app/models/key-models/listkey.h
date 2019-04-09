#pragma once
#include <QByteArray>
#include "listlikekey.h"

class ListKeyModel : public ListLikeKeyModel {
 public:
  ListKeyModel(QSharedPointer<RedisClient::Connection> connection,
               QByteArray fullPath, int dbIndex, long long ttl);

  QString type() override;

  void addRow(const QVariantMap &, ValueEditor::Model::Callback c) override;
  virtual void updateRow(int rowIndex, const QVariantMap &,
                         ValueEditor::Model::Callback c) override;
  void removeRow(int, ValueEditor::Model::Callback c) override;

 private:
  void verifyListItemPosistion(int row, Callback c);
  void addListRow(const QByteArray &value, Callback c);
  void setListRow(int pos, const QByteArray &value, Callback c);
  void deleteListRow(int count, const QByteArray &value, Callback c);
};
