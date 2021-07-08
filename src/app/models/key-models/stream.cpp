#include "stream.h"
#include <QJsonDocument>

StreamKeyModel::StreamKeyModel(
    QSharedPointer<RedisClient::Connection> connection, QByteArray fullPath,
    int dbIndex, long long ttl)
    : KeyModel(connection, fullPath, dbIndex, ttl, "XLEN", QByteArray()) {}

QString StreamKeyModel::type() { return "stream"; }

QStringList StreamKeyModel::getColumnNames() {
  return QStringList() << "rowNumber"
                       << "id"
                       << "value";
}

QHash<int, QByteArray> StreamKeyModel::getRoles() {
  QHash<int, QByteArray> roles;
  roles[Roles::RowNumber] = "rowNumber";
  roles[Roles::ID] = "id";
  roles[Roles::Value] = "value";
  return roles;
}

QVariant StreamKeyModel::getData(int rowIndex, int dataRole) {
  if (!isRowLoaded(rowIndex)) return QVariant();

  switch (dataRole) {
    case Value:
      return QJsonDocument::fromVariant(m_rowsCache[rowIndex].second)
          .toJson(QJsonDocument::Compact);
    case ID:
      return m_rowsCache[rowIndex].first;
    case RowNumber:
      return rowIndex;
  }

  return QVariant();
}

void StreamKeyModel::addRow(const QVariantMap &row,
                            ValueEditor::Model::Callback c) {
  if (!isRowValid(row)) {
    c(QCoreApplication::translate("RDM", "Invalid row"));
    return;
  }

  QList<QByteArray> cmd = {"XADD", m_keyFullPath, row["id"].toByteArray()};

  QJsonParseError err;
  QJsonDocument jsonValues =
      QJsonDocument::fromJson(row["value"].toByteArray(), &err);

  if (err.error != QJsonParseError::NoError || !jsonValues.isObject()) {
    return c(QCoreApplication::translate("RDM", "Invalid row"));
  }

  auto valuesObject = jsonValues.object();

  for (auto key : valuesObject.keys()) {
    cmd.append(key.toUtf8());
    cmd.append(valuesObject[key].toVariant().toString().toUtf8());
  }

  executeCmd(cmd, c);
}

void StreamKeyModel::updateRow(int, const QVariantMap &,
                               ValueEditor::Model::Callback) {
    //NOTE(u_glide): Redis Streams doesn't support editing (yet?)
}

void StreamKeyModel::removeRow(int i, ValueEditor::Model::Callback c) {
  if (!isRowLoaded(i)) return;

  executeCmd({"XDEL", m_keyFullPath, m_rowsCache[i].first}, c);
}

void StreamKeyModel::loadRowsCount(ValueEditor::Model::Callback c)
{
  executeCmd(
      {"XINFO", "STREAM", m_keyFullPath}, c,
      [this](RedisClient::Response r, Callback c) {
        auto info = r.value().toList();
        auto it = info.begin();

        while (it != info.end()) {
          if (!it->canConvert(QMetaType::QByteArray)) {
            continue;
          }

          QByteArray propertyName = it->toByteArray();

          it++;

          if (it == info.end())
              break;

          if (propertyName == QByteArray("length")) {
            m_rowCount = it->toLongLong();
          } else if (propertyName == QByteArray("first-entry") ||
                     propertyName == QByteArray("last-entry")) {
            auto list = it->toList();

            if (list.size() > 0) {
              m_filters[QString::fromLatin1(propertyName)] = list[0];
            }
          }

          it++;
        }

        c(QString());
      },
      RedisClient::Response::Type::Array);
}

int StreamKeyModel::addLoadedRowsToCache(const QVariantList &rows,
                                         QVariant rowStartId) {
  QList<QPair<QByteArray, QVariant>> result;

  for (QVariantList::const_iterator item = rows.begin(); item != rows.end();
       ++item) {
    QPair<QByteArray, QVariant> value;
    auto rowValues = item->toList();
    value.first = rowValues[0].toByteArray();

    QVariantList valuesList = rowValues[1].toList();
    QVariantMap mappedVal;

    for (QVariantList::const_iterator valItem = valuesList.begin();
         valItem != valuesList.end(); ++valItem) {               
      auto valKey = valItem->toByteArray();
      valItem++;

      // NOTE(u_glide): Temporary workaround for https://bugreports.qt.io/browse/QTBUG-84739
      mappedVal[valKey] = QString::fromUtf8(valItem->toByteArray());
    }

    value.second = mappedVal;
    result.push_back(value);
  }

  auto rowStart = rowStartId.toLongLong();
  m_rowsCache.addLoadedRange({rowStart, rowStart + result.size() - 1}, result);

  return result.size();
}

QList<QByteArray> StreamKeyModel::getRangeCmd(QVariant rowStartId,
                                              unsigned long count) {
  QList<QByteArray> cmd;
  cmd << "XREVRANGE" << m_keyFullPath;

  if (filter("end").isNull()) {  // end
    cmd << "+";
  } else {
    cmd << QString::number(filter("end").toLongLong()).toLatin1();
  }

  if (filter("start").isNull()) {  // start
    unsigned long rowStart = rowStartId.toULongLong();

    if (isRowLoaded(rowStart - 1)) {
      cmd << m_rowsCache[rowStart - 1].first;
    } else {
       cmd << "-";
    }
  } else {
    cmd << QString::number(filter("start").toLongLong()).toLatin1();
  }

  return cmd << "COUNT" << QString::number(count).toLatin1();
}
