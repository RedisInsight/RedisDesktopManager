#include "rejsonkey.h"
#include <qredisclient/connection.h>

ReJSONKeyModel::ReJSONKeyModel(
    QSharedPointer<RedisClient::Connection> connection, QByteArray fullPath,
    int dbIndex, long long ttl)
    : KeyModel(connection, fullPath, dbIndex, ttl) {}

QString ReJSONKeyModel::type() { return "ReJSON"; }

QStringList ReJSONKeyModel::getColumnNames() {
  return QStringList() << "value";
}

QHash<int, QByteArray> ReJSONKeyModel::getRoles() {
  QHash<int, QByteArray> roles;
  roles[Roles::Value] = "value";
  return roles;
}

QVariant ReJSONKeyModel::getData(int rowIndex, int dataRole) {
  if (!isRowLoaded(rowIndex)) return QVariant();

  if (dataRole == Roles::Value) return m_rowsCache[rowIndex];

  return QVariant();
}

void ReJSONKeyModel::updateRow(int rowIndex, const QVariantMap& row,
                               Callback c) {
  if (rowIndex > 0 || !isRowValid(row)) {
    qDebug() << "Row is not valid";
    return;
  }

  QByteArray value = row.value("value").toByteArray();

  if (value.isEmpty()) return;

  auto responseHandler = [this, value](RedisClient::Response r, Callback c) {
    if (r.isOkMessage()) {
      m_rowsCache.clear();
      m_rowsCache.addLoadedRange({0, 0}, (QList<QByteArray>() << value));
      return c(QString());
    } else {
      return c(r.value().toString());
    }
  };

  executeCmd({"JSON.SET", m_keyFullPath, ".", value}, c, responseHandler);
}

void ReJSONKeyModel::addRow(const QVariantMap& row, Callback c) {
  updateRow(0, row, c);
}

void ReJSONKeyModel::loadRows(QVariant, unsigned long,
                              LoadRowsCallback callback) {
  auto onConnectionError = [callback](const QString& err) {
    return callback(err, 0);
  };

  auto responseHandler = [this, callback](RedisClient::Response r, Callback) {
    m_rowsCache.clear();
    m_rowsCache.push_back(r.value().toByteArray());

    callback(QString(), 1);
  };

  executeCmd({"JSON.GET", m_keyFullPath}, onConnectionError, responseHandler,
             RedisClient::Response::String);
}

void ReJSONKeyModel::removeRow(int, Callback c) {
  m_rowCount--;
  setRemovedIfEmpty();
}
