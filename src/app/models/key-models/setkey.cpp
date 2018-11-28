#include "setkey.h"
#include <qredisclient/connection.h>

SetKeyModel::SetKeyModel(QSharedPointer<RedisClient::Connection> connection,
                         QByteArray fullPath, int dbIndex, long long ttl)
    : ListLikeKeyModel(connection, fullPath, dbIndex, ttl, "SCARD", "SSCAN") {}

QString SetKeyModel::type() { return "set"; }

void SetKeyModel::updateRow(int rowIndex, const QVariantMap &row, Callback c) {
  if (!isRowLoaded(rowIndex) || !isRowValid(row)) {
    emit m_notifier->error(QCoreApplication::translate("RDM", "Invalid row"));
    return;
  }

  QByteArray cachedRow = m_rowsCache[rowIndex];
  QByteArray newRow(row["value"].toByteArray());

  auto onRowAdded = [this, c, rowIndex, newRow](const QString &err) {
    if (err.isEmpty()) m_rowsCache.replace(rowIndex, newRow);
    return c(err);
  };

  deleteSetRow(cachedRow, [this, c, newRow, onRowAdded](const QString &err) {
    if (err.size() > 0) return c(err);

    addSetRow(newRow, onRowAdded);
  });
}

void SetKeyModel::addRow(const QVariantMap &row, Callback c) {
  if (!isRowValid(row)) {
    return c(QCoreApplication::translate("RDM", "Invalid row"));
  }

  addSetRow(row["value"].toByteArray(), [this, c](const QString &err) {
    if (err.isEmpty()) {
      m_rowCount++;
    }

    return c(err);
  });
}

void SetKeyModel::removeRow(int i, Callback c) {
  if (!isRowLoaded(i)) return;

  QByteArray value = m_rowsCache[i];
  deleteSetRow(value, [this, c, i](const QString &err) {
    if (err.isEmpty()) {
      m_rowCount--;
      m_rowsCache.removeAt(i);

      setRemovedIfEmpty();
    }

    return c(err);
  });
}

void SetKeyModel::addSetRow(const QByteArray &value, Callback c) {
  executeCmd({"SADD", m_keyFullPath, value}, c);
}

void SetKeyModel::deleteSetRow(const QByteArray &value, Callback c) {
  executeCmd({"SREM", m_keyFullPath, value}, c);
}
