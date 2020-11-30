#pragma once
#include <qredisclient/connection.h>
#include <QEnableSharedFromThis>
#include <QHash>
#include <QObject>
#include <QString>
#include <functional>
#include "exception.h"

namespace ValueEditor {

class ModelSignals : public QObject {
  Q_OBJECT
 public:
  ModelSignals() {}
 signals:
  void removed();
  void error(const QString&);
};

class Model : public QEnableSharedFromThis<Model> {
 public:
  typedef std::function<void(const QString&)> Callback;

  Model() {}
  virtual QString getKeyName() = 0;
  virtual QString getKeyTitle(int limit = -1) = 0;

  virtual QString type() = 0;
  virtual long long getTTL() = 0;
  virtual QStringList getColumnNames() = 0;
  virtual QHash<int, QByteArray> getRoles() = 0;
  virtual QVariant getData(int rowIndex, int dataRole) = 0;

  virtual void setKeyName(const QByteArray&, Callback) = 0;  // async
  virtual void setTTL(const long long, Callback) = 0;        // async
  virtual void persistKey(Callback) = 0;        // async
  virtual void removeKey(Callback) = 0;

  // rows operations
  virtual void addRow(const QVariantMap&, Callback) = 0;
  virtual void updateRow(int rowIndex, const QVariantMap&,
                         Callback) = 0;  // async
  virtual unsigned long rowsCount() = 0;

  //filters
  virtual QVariant filter(const QString& key) const = 0;
  virtual void setFilter(const QString&, QVariant) = 0;

  typedef std::function<void(const QString&, unsigned long)> LoadRowsCallback;
  virtual void loadRows(QVariant rowStart, unsigned long count,
                        LoadRowsCallback c) = 0;  // async

  virtual void clearRowCache() = 0;
  virtual void removeRow(int, Callback) = 0;  // async
  virtual bool isRowLoaded(int) = 0;
  virtual bool isMultiRow() const = 0;
  virtual void loadRowsCount(Callback callback) = 0;

  virtual QSharedPointer<ModelSignals> getConnector() const = 0;
  virtual QSharedPointer<RedisClient::Connection> getConnection() const = 0;
  virtual unsigned int dbIndex() const = 0;

  virtual ~Model() {}
};

}  // namespace ValueEditor
