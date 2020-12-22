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

  int dbRowIndex = rowIndex;

  if (isReverseOrder()) {
    dbRowIndex = -rowIndex - 1;
  }

  QByteArray newRow(row["value"].toByteArray());

  auto afterRowUpdate = [this, rowIndex, newRow, c](const QString &err) {
    if (err.isEmpty()) m_rowsCache.replace(rowIndex, newRow);

    return c(err);
  };

  verifyListItemPosition(dbRowIndex, [this, dbRowIndex, c, newRow,
                                       afterRowUpdate](const QString &err) {
    if (err.size() > 0) return c(err);

    setListRow(dbRowIndex, newRow, afterRowUpdate);
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

  int dbRowIndex = i;

  if (isReverseOrder()) {
    dbRowIndex = -i - 1;
  }

  verifyListItemPosition(
      dbRowIndex, [this, dbRowIndex, c, onItemHidding](const QString &err) {
        if (err.size() > 0) return c(err);

        // Replace value by system string
        setListRow(dbRowIndex, LIST_ITEM_REMOVAL_STUB, onItemHidding);
      });
}

QList<QByteArray> ListKeyModel::getRangeCmd(QVariant rowStartId, unsigned long count)
{
    if (isReverseOrder()) {
        long rowStart = -rowStartId.toLongLong() - 1;
        long rowStop = rowStart - count + 1;

        QList<QByteArray> cmd {m_rowsLoadCmd, m_keyFullPath,
                              QString::number(rowStop).toLatin1(),
                              QString::number(rowStart).toLatin1()};
        return cmd;
    } else {
        return KeyModel::getRangeCmd(rowStartId, count);
    }
}

int ListKeyModel::addLoadedRowsToCache(const QVariantList &rows,
                                       QVariant rowStartId) {
  if (isReverseOrder()) {
    return ListLikeKeyModel::addLoadedRowsToCache(
        QList<QVariant>(rows.rbegin(), rows.rend()), rowStartId);
  } else {
    return ListLikeKeyModel::addLoadedRowsToCache(rows, rowStartId);
  }
}

void ListKeyModel::verifyListItemPosition(int row, Callback c) {
  auto verifyResponse = [this, row](RedisClient::Response r, Callback c) {
    QVariantList currentState = r.value().toList();        
    QByteArray cachedValue;

    if (isReverseOrder()) {
        cachedValue = m_rowsCache[-row - 1];
    } else {
        cachedValue = m_rowsCache[row];
    }

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

bool ListKeyModel::isReverseOrder() const
{
    return m_filters.value("order", "default") == "reverse";
}
