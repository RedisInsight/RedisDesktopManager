#include "rejsonkey.h"
#include <qredisclient/connection.h>

ReJSONKeyModel::ReJSONKeyModel(
    QSharedPointer<RedisClient::Connection> connection, QByteArray fullPath,
    int dbIndex, long long ttl)
    : KeyModel(connection, fullPath, dbIndex, ttl) {}

QString ReJSONKeyModel::type() { return "ReJSON"; }

QStringList ReJSONKeyModel::getColumnNames() {
  return QStringList();  // Single value type - No columns
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

  RedisClient::Response result;
  try {
    result = m_connection->commandSync({"JSON.SET", m_keyFullPath, ".", value},
                                       m_dbIndex);
  } catch (const RedisClient::Connection::Exception& e) {
    emit m_notifier->error(
        QCoreApplication::translate("RDM", "Connection error: ") +
        QString(e.what()));
    return;
  }

  if (result.isOkMessage()) {
    m_rowsCache.clear();
    m_rowsCache.addLoadedRange({0, 0}, (QList<QByteArray>() << value));
  }
}

void ReJSONKeyModel::addRow(const QVariantMap& row, Callback c) {
  updateRow(0, row, c);
}

void ReJSONKeyModel::loadRows(
    QVariant, unsigned long,
    std::function<void(const QString&, unsigned long)> callback) {
  try {
    m_connection->command(
        {"JSON.GET", m_keyFullPath}, getConnector().data(),
        [this, callback](RedisClient::Response r, QString e) {
          if (r.type() != RedisClient::Response::String || !e.isEmpty()) {
            return callback(QString("Cannot load value"), 0);
          }

          m_rowsCache.clear();
          m_rowsCache.push_back(r.value().toByteArray());

          callback(QString(), 1);
        },
        m_dbIndex);
  } catch (const RedisClient::Connection::Exception& e) {
    emit m_notifier->error(
        QCoreApplication::translate("RDM", "Connection error: ") +
        QString(e.what()));
  }
}

void ReJSONKeyModel::removeRow(int, Callback c) {
  m_rowCount--;
  setRemovedIfEmpty();
}
