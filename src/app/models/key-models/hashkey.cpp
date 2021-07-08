#include "hashkey.h"
#include <qredisclient/connection.h>
#include <QObject>

HashKeyModel::HashKeyModel(QSharedPointer<RedisClient::Connection> connection,
                           QByteArray fullPath, int dbIndex, long long ttl)
    : KeyModel(connection, fullPath, dbIndex, ttl, "HLEN", "HSCAN") {}

QString HashKeyModel::type() { return "hash"; }

QStringList HashKeyModel::getColumnNames() {
  return QStringList() << "rowNumber"
                       << "key"
                       << "value";
}

QHash<int, QByteArray> HashKeyModel::getRoles() {
  QHash<int, QByteArray> roles;
  roles[Roles::RowNumber] = "rowNumber";
  roles[Roles::Key] = "key";
  roles[Roles::Value] = "value";
  return roles;
}

QVariant HashKeyModel::getData(int rowIndex, int dataRole) {
  if (!isRowLoaded(rowIndex)) return QVariant();

  QPair<QByteArray, QByteArray> row = m_rowsCache[rowIndex];

  if (dataRole == Roles::Key)
    return row.first;
  else if (dataRole == Roles::Value)
    return row.second;
  else if (dataRole == Roles::RowNumber)
    return rowIndex;

  return QVariant();
}

void HashKeyModel::updateRow(int rowIndex, const QVariantMap &row, Callback c) {
  if (!isRowLoaded(rowIndex) || !isRowValid(row)) {
    c(QCoreApplication::translate("RDM", "Invalid row"));
    return;
  }

  QPair<QByteArray, QByteArray> cachedRow = m_rowsCache[rowIndex];

  bool keyChanged = cachedRow.first != row["key"].toString();
  bool valueChanged = cachedRow.second != row["value"].toString();

  QPair<QByteArray, QByteArray> newRow(
      (keyChanged) ? row["key"].toByteArray() : cachedRow.first,
      (valueChanged) ? row["value"].toByteArray() : cachedRow.second);

  auto afterValueUpdate = [this, c, rowIndex, newRow](const QString &err) {
    if (err.isEmpty()) m_rowsCache.replace(rowIndex, newRow);

    return c(err);
  };

  if (keyChanged) {
    deleteHashRow(cachedRow.first,
                  [this, c, newRow, afterValueUpdate](const QString &err) {
                    if (err.size() > 0) return c(err);

                    setHashRow(newRow.first, newRow.second, afterValueUpdate);
                  });
  } else {
    setHashRow(newRow.first, newRow.second, afterValueUpdate);
  }
}

void HashKeyModel::addRow(const QVariantMap &row, Callback c) {
  if (!isRowValid(row)) {
    c(QCoreApplication::translate("RDM", "Invalid row"));
    return;
  }

  setHashRow(
      row["key"].toByteArray(), row["value"].toByteArray(),
      [this, c](const QString &err) {
        if (err.isEmpty()) m_rowCount++;
        return c(err);
      },
      false);
}

void HashKeyModel::removeRow(int i, Callback c) {
  if (!isRowLoaded(i)) return;

  QPair<QByteArray, QByteArray> row = m_rowsCache[i];

  deleteHashRow(row.first, [this, i, c](const QString &err) {
    if (err.isEmpty()) {
      m_rowCount--;
      m_rowsCache.removeAt(i);
      setRemovedIfEmpty();
    }

    return c(err);
  });
}

void HashKeyModel::setHashRow(const QByteArray &hashKey,
                              const QByteArray &hashValue, Callback c,
                              bool updateIfNotExist) {
  QList<QByteArray> rawCmd{(updateIfNotExist) ? "HSET" : "HSETNX",
                           m_keyFullPath, hashKey, hashValue};

  executeCmd(rawCmd, c,
             [updateIfNotExist](RedisClient::Response r, Callback c) {
               if (updateIfNotExist == false && r.value().toInt() == 0) {
                 return c(QCoreApplication::translate(
                     "RDM", "Value with the same key already exists"));
               } else {
                 return c(QString());
               }
             });
}

void HashKeyModel::deleteHashRow(const QByteArray &hashKey, Callback c) {
  executeCmd({"HDEL", m_keyFullPath, hashKey}, c);
}

int HashKeyModel::addLoadedRowsToCache(const QVariantList &rows,
                                       QVariant rowStartId) {
  QList<QPair<QByteArray, QByteArray>> result;

  for (QVariantList::const_iterator item = rows.begin(); item != rows.end();
       ++item) {
    QPair<QByteArray, QByteArray> value;
    value.first = item->toByteArray();
    ++item;

    if (item == rows.end()) {
      emit m_notifier->error(QCoreApplication::translate(
          "RDM", "Data was loaded from server partially."));
      return 0;
    }

    value.second = item->toByteArray();
    result.push_back(value);
  }

  auto rowStart = rowStartId.toLongLong();
  m_rowsCache.addLoadedRange({rowStart, rowStart + result.size() - 1}, result);

  return result.size();
}
