#include "bfkey.h"

#include <QJsonDocument>

BloomFilterKeyModel::BloomFilterKeyModel(
    QSharedPointer<RedisClient::Connection> connection, QByteArray fullPath,
    int dbIndex, long long ttl, QString filterFamily)
    : KeyModel(connection, fullPath, dbIndex, ttl), m_type(filterFamily) {}

QString BloomFilterKeyModel::type() { return m_type; }

QStringList BloomFilterKeyModel::getColumnNames() {
  return QStringList() << "value";
}

QHash<int, QByteArray> BloomFilterKeyModel::getRoles() {
  QHash<int, QByteArray> roles;
  roles[Roles::Value] = "value";
  return roles;
}

QVariant BloomFilterKeyModel::getData(int rowIndex, int dataRole) {
  if (rowIndex > 0 || !isRowLoaded(rowIndex)) return QVariant();
  if (dataRole == Roles::Value)
    return QJsonDocument::fromVariant(m_rowsCache[rowIndex])
        .toJson(QJsonDocument::Compact);

  return QVariant();
}

void BloomFilterKeyModel::addRow(const QVariantMap& row, Callback c) {
  QByteArray value = row.value("value").toByteArray();

  executeCmd({QString("%1.ADD").arg(m_type).toLatin1(), m_keyFullPath, value},
             [this, c](const QString& err) {
               m_rowCount++;
               return c(err);
             });
}

void BloomFilterKeyModel::loadRows(QVariant, unsigned long,
                                   LoadRowsCallback callback) {
  auto onConnectionError = [callback](const QString& err) {
    return callback(err, 0);
  };

  auto responseHandler = [this, callback](const RedisClient::Response& r, Callback) {
    m_rowsCache.clear();
    auto value = r.value().toList();

    QVariantMap row;

    for (auto item = value.cbegin(); item != value.cend(); ++item) {
      auto key = item->toByteArray();
      ++item;

      if (item == value.cend()) {
        emit m_notifier->error(QCoreApplication::translate(
            "RESP", "Data was loaded from server partially."));
        break;
      }

      auto keyVal = item->toByteArray();
      row[key] = keyVal;
    }

    m_rowsCache.push_back(row);
    callback(QString(), 1);
  };

  executeCmd({QString("%1.INFO").arg(m_type).toLatin1(), m_keyFullPath},
             onConnectionError, responseHandler, RedisClient::Response::Array);
}
