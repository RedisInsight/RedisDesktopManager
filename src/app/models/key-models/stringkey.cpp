#include "stringkey.h"
#include <qredisclient/connection.h>

StringKeyModel::StringKeyModel(
    QSharedPointer<RedisClient::Connection> connection, QByteArray fullPath,
    int dbIndex, long long ttl)
    : KeyModel(connection, fullPath, dbIndex, ttl), m_type("string") {}

QString StringKeyModel::type() { return m_type; }

QStringList StringKeyModel::getColumnNames() {
  return QStringList() << "value";
}

QHash<int, QByteArray> StringKeyModel::getRoles() {
  QHash<int, QByteArray> roles;
  roles[Roles::Value] = "value";
  return roles;
}

QVariant StringKeyModel::getData(int rowIndex, int dataRole) {
  if (rowIndex > 0 || !isRowLoaded(rowIndex)) return QVariant();
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
  if (m_type == "hyperloglog") {
      QByteArray value = row.value("value").toByteArray();

      executeCmd(
          {"PFADD", m_keyFullPath, value}, [this, c](const QString& err) {
            m_rowCount++;
            return c(err);
          });
  } else {
    updateRow(0, row, c);
  }
}

void StringKeyModel::loadRows(QVariant, unsigned long,
                              LoadRowsCallback callback) {
  auto onConnectionError = [callback](const QString& err) {
    return callback(err, 0);
  };

  auto responseHandler = [this, callback](RedisClient::Response r, Callback) {
    m_rowsCache.clear();

    QByteArray value = r.value().toByteArray();

    m_rowsCache.push_back(value);
    m_rowCount = 1;

    // Detect HyperLogLog
    if (value.startsWith("HYLL")) {
      executeCmd(
          {"PFCOUNT", m_keyFullPath}, [callback](const QString&) { callback(QString(), 1); },
          [this, callback](RedisClient::Response r, Callback) {
            m_type = "hyperloglog";
            m_rowCount = r.value().toUInt();
            callback(QString(), m_rowCount);
          },
          RedisClient::Response::Integer);
    } else {
      callback(QString(), 1);
    }
  };

  executeCmd({"GET", m_keyFullPath}, onConnectionError, responseHandler,
             RedisClient::Response::String);
}

void StringKeyModel::removeRow(int, Callback) {
  m_rowCount--;
  setRemovedIfEmpty();
}
