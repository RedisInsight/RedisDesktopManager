#include "tairhashkey.h"
#include <qredisclient/connection.h>
#include <QObject>

TairHashKeyModel::TairHashKeyModel(QSharedPointer<RedisClient::Connection> connection,
                           QByteArray fullPath, int dbIndex, long long ttl)
    : KeyModel(connection, fullPath, dbIndex, ttl, "EXHLEN", "EXHSCAN") {}

QString TairHashKeyModel::type() { return "tairhash"; }

QStringList TairHashKeyModel::getColumnNames() {
  return QStringList() << "rowNumber"
                       << "key"
                       << "value"
                       << "metadata";
}

QHash<int, QByteArray> TairHashKeyModel::getRoles() {
  QHash<int, QByteArray> roles;
  roles[Roles::RowNumber] = "rowNumber";
  roles[Roles::Key] = "key";
  roles[Roles::Value] = "value";
  roles[Roles::TTL] = "ttl";
  roles[Roles::Version] = "version";
  return roles;
}

QVariant TairHashKeyModel::getData(int rowIndex, int dataRole) {
  if (!isRowLoaded(rowIndex)) return QVariant();

  auto row = m_rowsCache[rowIndex];

  if (dataRole == Roles::Key)
    return row.first;
  else if (dataRole == Roles::Value)
    return row.second.value;
  else if (dataRole == Roles::TTL)
    return row.second.ttl;
  else if (dataRole == Roles::Version)
    return row.second.version;
  else if (dataRole == Roles::RowNumber)
    return rowIndex;

  return QVariant();
}

void TairHashKeyModel::updateRow(int rowIndex, const QVariantMap &row, Callback c) {
  if (!isRowLoaded(rowIndex) || !isRowValid(row)) {
    c(QCoreApplication::translate("RESP", "Invalid row"));
    return;
  }

  auto cachedRow = m_rowsCache[rowIndex];

  bool keyChanged = cachedRow.first != row["key"].toByteArray();
  bool valueChanged = cachedRow.second.value != row["value"].toByteArray();

  TairHashValue newValue;

  if (valueChanged) {
    newValue = {row["value"].toByteArray(), cachedRow.second.ttl,
                cachedRow.second.version};
  } else {
    newValue = cachedRow.second;
  }

  QPair<QByteArray, TairHashValue> newRow(
      (keyChanged) ? row["key"].toByteArray() : cachedRow.first, newValue
     );

  auto afterValueUpdate = [this, c, rowIndex, newRow](const QString &err) {
    if (err.isEmpty()) {
        m_rowsCache.replace(rowIndex, newRow);
        loadMetadata(newRow.first, rowIndex);
    }

    return c(err);
  };

  if (keyChanged) {
    deleteHashRow(cachedRow.first,
                  [this, c, newRow, afterValueUpdate](const QString &err) {
                    if (err.size() > 0) return c(err);

                    setHashRow(newRow.first, newRow.second.value, afterValueUpdate);
                  });
  } else {
    setHashRow(newRow.first, newRow.second.value, afterValueUpdate);
  }
}

void TairHashKeyModel::addRow(const QVariantMap &row, Callback c) {
  if (!isRowValid(row)) {
    c(QCoreApplication::translate("RESP", "Invalid row"));
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

void TairHashKeyModel::removeRow(int i, Callback c) {
  if (!isRowLoaded(i)) return;

  auto row = m_rowsCache[i];

  deleteHashRow(row.first, [this, i, c](const QString &err) {
    if (err.isEmpty()) {
      m_rowCount--;
      m_rowsCache.removeAt(i);
      setRemovedIfEmpty();
    }

    return c(err);
  });
}

void TairHashKeyModel::setHashRow(const QByteArray &hashKey,
                              const QByteArray &hashValue, Callback c,
                              bool updateIfNotExist) {
  QList<QByteArray> rawCmd{(updateIfNotExist) ? "EXHSET" : "EXHSETNX",
                           m_keyFullPath, hashKey, hashValue};

  executeCmd(rawCmd, c,
             [updateIfNotExist](RedisClient::Response r, Callback c) {
               if (updateIfNotExist == false && r.value().toInt() == 0) {
                 return c(QCoreApplication::translate(
                     "RESP", "Value with the same key already exists"));
               } else {
                 return c(QString());
               }
             });
}

void TairHashKeyModel::deleteHashRow(const QByteArray &hashKey, Callback c) {
    executeCmd({"EXHDEL", m_keyFullPath, hashKey}, c);
}

void TairHashKeyModel::loadMetadata(const QByteArray &hashKey, int rowIndex) {
  executeCmd({"EXHVER", m_keyFullPath, hashKey}, Callback{},
             [rowIndex, this](RedisClient::Response r, Callback) {
               auto row = m_rowsCache.getRow(rowIndex);
               row.second.version = r.value().toLongLong();
               m_rowsCache.replace(rowIndex, row);
             });

  executeCmd({"EXHTTL", m_keyFullPath, hashKey}, Callback{},
             [rowIndex, this](RedisClient::Response r, Callback) {
               auto row = m_rowsCache.getRow(rowIndex);
               row.second.ttl = r.value().toLongLong();
               m_rowsCache.replace(rowIndex, row);
             });
}

int TairHashKeyModel::addLoadedRowsToCache(const QVariantList &rows,
                                       QVariant rowStartId) {
  QList<QPair<QByteArray, TairHashValue>> result;

  for (QVariantList::const_iterator item = rows.begin(); item != rows.end();
       ++item) {
    QPair<QByteArray, TairHashValue> value;
    QByteArray hashKey = item->toByteArray();
    value.first = hashKey;
    ++item;

    int currentIndex = result.size();

    if (item == rows.end()) {
      emit m_notifier->error(QCoreApplication::translate(
          "RESP", "Data was loaded from server partially."));
      return 0;
    }

    value.second = TairHashValue{item->toByteArray(), -1, -1};
    result.push_back(value);

    loadMetadata(value.first, currentIndex);
  }

  auto rowStart = rowStartId.toLongLong();
  m_rowsCache.addLoadedRange({rowStart, rowStart + result.size() - 1}, result);

  return result.size();
}
