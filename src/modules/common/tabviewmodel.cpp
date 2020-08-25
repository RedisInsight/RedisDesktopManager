#include "tabviewmodel.h"
#include <QQmlEngine>

TabViewModel::TabViewModel(const ModelFactory &modelFactory)
    : m_modelFactory(modelFactory) {}

QModelIndex TabViewModel::index(int row, int, const QModelIndex &) const {
  return createIndex(row, 0);
}

int TabViewModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  return m_models.count();
}

QVariant TabViewModel::data(const QModelIndex &index, int role) const {
  if (!isIndexValid(index)) return QVariant();

  QSharedPointer<TabModel> model = m_models.at(index.row());

  if (model.isNull()) return QVariant();

  switch (role) {
    case tabIndex:
      return index.row();
    case tabName:
      return model->getName();
    case tabModel:
      QObject *modelPtr =
          static_cast<QObject *>(m_models.at(index.row()).data());
      QQmlEngine::setObjectOwnership(modelPtr, QQmlEngine::CppOwnership);

      return QVariant::fromValue(modelPtr);
  }

  return QVariant();
}

QHash<int, QByteArray> TabViewModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[tabIndex] = "tabIndex";
  roles[tabName] = "tabName";
  roles[tabModel] = "tabModel";
  return roles;
}

void TabViewModel::closeTab(int i) {
  if (!isIndexValid(index(i, 0))) return;

  beginRemoveRows(QModelIndex(), i, i);
  m_models.at(i)->disconnect();
  m_models.removeAt(i);
  endRemoveRows();
}

int TabViewModel::tabsCount() const { return m_models.count(); }

void TabViewModel::openTab(QSharedPointer<RedisClient::Connection> connection,
                           int dbIndex, QList<QByteArray> initCmd) {
  beginInsertRows(QModelIndex(), m_models.count(), m_models.count());
  m_models.append(m_modelFactory(connection, dbIndex, initCmd));
  emit changeCurrentTab(m_models.size() - 1);
  endInsertRows();
}

void TabViewModel::closeAllTabsWithConnection(
    QSharedPointer<RedisClient::Connection> connection) {
  for (int index = 0; 0 <= index && index < m_models.size(); index++) {
    auto model = m_models.at(index);

    if (model->getConnection() == connection) {
      beginRemoveRows(QModelIndex(), index, index);
      m_models.removeAt(index);
      endRemoveRows();
      index--;
    }
  }
}

bool TabViewModel::isIndexValid(const QModelIndex &index) const {
  return 0 <= index.row() && index.row() < rowCount();
}
