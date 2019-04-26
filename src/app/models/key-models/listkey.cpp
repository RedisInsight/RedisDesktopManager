#include "listkey.h"
#include <qredisclient/connection.h>

const static QByteArray LIST_ITEM_REMOVAL_STUB("---VALUE_REMOVED_BY_RDM---");

ListKeyModel::ListKeyModel(QSharedPointer<RedisClient::Connection> connection,
                           QByteArray fullPath, int dbIndex, long long ttl)
    : ListLikeKeyModel(connection, fullPath, dbIndex, ttl, "LLEN", "LRANGE") {}

QString ListKeyModel::type() { return "list"; }

void ListKeyModel::updateRow(int rowIndex, const QVariantMap &row, Callback c) {
  if (!isRowLoaded(rowIndex) || !isRowValid(row)) {
    return c(QCoreApplication::translate("RDM", "Invalid row"));
  }

  QByteArray newRow(row["value"].toByteArray());

  auto afterRowUpdate = [this, rowIndex, newRow, c](const QString &err) {
    if (err.isEmpty()) m_rowsCache.replace(rowIndex, newRow);

    return c(err);
  };

  verifyListItemPosistion(rowIndex, [this, rowIndex, c, newRow,
                                     afterRowUpdate](const QString &err) {
    if (err.size() > 0) return c(err);

    setListRow(rowIndex, newRow, afterRowUpdate);
  });
}

void ListKeyModel::addRow(const QVariantMap &row, Callback c) {
  if (!isRowValid(row)) {
    emit m_notifier->error(QCoreApplication::translate("RDM", "Invalid row"));
    return;
  }

  addListRow(row["value"].toByteArray(), [this, c](const QString &err) {
    if (err.isEmpty()) m_rowCount++;

    return c(err);
  });
}

void ListKeyModel::removeRow(int i, ValueEditor::Model::Callback c) {
  if (!isRowLoaded(i)) return;

  auto onItemRemoval = [this, c, i](const QString &err) {
    if (err.isEmpty()) {
      m_rowCount--;
      m_rowsCache.removeAt(i);
      setRemovedIfEmpty();
    };

    return c(err);
  };

  auto onItemHidding = [this, c, onItemRemoval](const QString &err) {
    if (err.size() > 0) return c(err);

    // Remove all system values from list
    deleteListRow(0, LIST_ITEM_REMOVAL_STUB, onItemRemoval);
  };

  verifyListItemPosistion(i, [this, i, c, onItemHidding](const QString &err) {
    if (err.size() > 0) return c(err);

    // Replace value by system string
    setListRow(i, LIST_ITEM_REMOVAL_STUB, onItemHidding);
  });
}

void ListKeyModel::verifyListItemPosistion(int row, Callback c) {
  auto verifyResponse = [this, row](RedisClient::Response r, Callback c) {
    QVariantList currentState = r.value().toList();
    QByteArray cachedValue = m_rowsCache[row];

    bool isChanged = currentState.size() != 1 ||
                     currentState[0].toByteArray() != QString(cachedValue);

    if (isChanged) {
      return c(QCoreApplication::translate("RDM",
                                           "The row has been changed on server."
                                           "Reload and try again."));
    } else {
      return c(QString());
    }
  };

  executeCmd({"LRANGE", m_keyFullPath, QString::number(row).toLatin1(),
              QString::number(row).toLatin1()},
             c, verifyResponse);
}

void ListKeyModel::addListRow(const QByteArray &value, Callback c) {
  executeCmd({"LPUSH", m_keyFullPath, value}, c);
}

void ListKeyModel::setListRow(int pos, const QByteArray &value, Callback c) {
  executeCmd({"LSET", m_keyFullPath, QString::number(pos).toLatin1(), value},
             c);
}

void ListKeyModel::deleteListRow(int count, const QByteArray &value,
                                 Callback c) {
  executeCmd({"LREM", m_keyFullPath, QString::number(count).toLatin1(), value},
             c);
}
