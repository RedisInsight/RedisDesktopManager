#include "stringkey.h"
#include <qredisclient/connection.h>

StringKeyModel::StringKeyModel(
    QSharedPointer<RedisClient::Connection> connection, QByteArray fullPath,
    int dbIndex, long long ttl)
    : KeyModel(connection, fullPath, dbIndex, ttl) {}

QString StringKeyModel::type() { return "string"; }

QStringList StringKeyModel::getColumnNames() {
  return QStringList();  // Single value type - No columns
}

QHash<int, QByteArray> StringKeyModel::getRoles() {
  QHash<int, QByteArray> roles;
  roles[Roles::Value] = "value";
  return roles;
}

QVariant StringKeyModel::getData(int rowIndex, int dataRole) {
  if (!isRowLoaded(rowIndex)) return QVariant();
  if (dataRole == Roles::Value) return m_rowsCache[rowIndex];

  return QVariant();
}

void StringKeyModel::updateRow(int rowIndex, const QVariantMap& row,
                               Callback c) {
  if (rowIndex > 0 || !isRowValid(row)) {
    qDebug() << "Row is not valid";
    return;
  }

  QByteArray value = row.value("value").toByteArray();

  if (value.isEmpty()) return;

  executeCmd(
      {"SET", m_keyFullPath, value}, [this, c, value](const QString& err) {
        if (err.isEmpty()) {
          m_rowsCache.clear();
          m_rowsCache.addLoadedRange({0, 0}, (QList<QByteArray>() << value));
        }

        return c(err);
      });
}

void StringKeyModel::addRow(const QVariantMap& row, Callback c) {
  updateRow(0, row, c);
}

void StringKeyModel::loadRows(QVariant, unsigned long,
                              LoadRowsCallback callback) {
  auto onConnectionError = [callback](const QString& err) {
    return callback(err, 0);
  };

  auto responseHandler = [this, callback](RedisClient::Response r, Callback) {
    m_rowsCache.clear();
    m_rowsCache.push_back(r.value().toByteArray());

    callback(QString(), 1);
  };

  executeCmd({"GET", m_keyFullPath}, onConnectionError, responseHandler,
             RedisClient::Response::String);
}

void StringKeyModel::removeRow(int, Callback) {
  m_rowCount--;
  setRemovedIfEmpty();
}
