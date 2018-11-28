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
  bool isActualPositionChanged(int row);
  void addListRow(const QByteArray &value);
  void setListRow(int pos, const QByteArray &value);
  void deleteListRow(int count, const QByteArray &value);
};
