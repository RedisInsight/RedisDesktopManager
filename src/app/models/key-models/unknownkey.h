#pragma once
#include "abstractkey.h"

#define EMPTY_METHOD override {qWarning() << "Operation is not supported";}

class UnknownKeyModel : public KeyModel<QByteArray> {
 public:
  UnknownKeyModel(QSharedPointer<RedisClient::Connection> connection,
                 QByteArray fullPath, int dbIndex, long long ttl, QString type);

  QString type() override { return m_type; }

  QStringList getColumnNames() override {
      return QStringList() << "";
  }
  QHash<int, QByteArray> getRoles() override {
    return QHash<int, QByteArray>();
  }

  QVariant getData(int, int) override {return QVariant();}

  void addRow(const QVariantMap&, Callback) EMPTY_METHOD

  virtual void updateRow(int, const QVariantMap&,
                         Callback) EMPTY_METHOD

  void loadRows(QVariant, unsigned long, LoadRowsCallback callback) override {
      return callback("unknown-data-type", 0);
  }

  void removeRow(int, Callback) EMPTY_METHOD

  virtual unsigned long rowsCount() override {
      return 0;
  }

 protected:
  int addLoadedRowsToCache(const QVariantList&, QVariant) override { return 1; }

 private:
  enum Roles { Value = Qt::UserRole + 1 };

  QString m_type;
};
