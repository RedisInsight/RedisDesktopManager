#include "tabsmodel.h"
#include <qredisclient/connection.h>
#include <qredisclient/utils/text.h>
#include <QCoreApplication>
#include <QDebug>
#include <QQmlEngine>
#include "connections-tree/items/keyitem.h"
#include "value-editor/valueviewmodel.h"

ValueEditor::TabsModel::TabsModel(QSharedPointer<AbstractKeyFactory> keyFactory)
    : m_keyFactory(keyFactory), m_currentTabIndex(0) {}

ValueEditor::TabsModel::~TabsModel() { m_viewModels.clear(); }

void ValueEditor::TabsModel::openTab(
    QSharedPointer<RedisClient::Connection> connection,
    QSharedPointer<ConnectionsTree::KeyItem> key, bool inNewTab) {
  auto loadingHandler = [this, inNewTab, key](QSharedPointer<Model> keyModel,
                                              const QString& error) {
    if (keyModel.isNull() || !error.isEmpty()) {
      emit tabError(-1, QString("<b>%1</b>:\n%2")
                            .arg(QCoreApplication::translate(
                                "RDM", "Cannot open value tab"))
                            .arg(error));
      return;
    }

    loadModel(keyModel, key.toWeakRef(), inNewTab);
  };

  try {
    m_keyFactory->loadKey(connection, key->getFullPath(), key->getDbIndex(),
                          loadingHandler);
  } catch (...) {
    emit tabError(-1, QCoreApplication::translate(
                          "RDM", "Connection error. Can't open value tab. "));
  }
}

void ValueEditor::TabsModel::closeDbKeys(
    QSharedPointer<RedisClient::Connection> connection, int dbIndex,
    const QRegExp& filter) {
  for (int index = 0; 0 <= index && index < m_viewModels.size(); index++) {
    auto model = m_viewModels.at(index)->model();

    if (model->getConnection() == connection && model->dbIndex() == dbIndex) {
      if (model->getKeyName().contains(filter)) {
        beginRemoveRows(QModelIndex(), index, index);
        auto model = m_viewModels[index];
        m_viewModels.removeAt(index);
        endRemoveRows();
        index--;
        model.clear();
      }
    }
  }
}

QModelIndex ValueEditor::TabsModel::index(int row, int column,
                                          const QModelIndex& parent) const {
  Q_UNUSED(parent);

  if (row < 0 || column < 0) return QModelIndex();

  return createIndex(row, 0);
}

int ValueEditor::TabsModel::rowCount(const QModelIndex&) const {
  return m_viewModels.count();
}

QVariant ValueEditor::TabsModel::data(const QModelIndex& index,
                                      int role) const {
  if (!isIndexValid(index)) return QVariant();

  QSharedPointer<Model> model = m_viewModels.at(index.row())->model();

  if (!model) return QVariant();

  switch (role) {
    case keyIndex:
      return index.row();
    case keyNameRole:
      return model->getKeyName();
    case keyDisplayName:
      return model->getKeyTitle();
    case keyTTL:
      return model->getTTL();
    case keyType:
      return model->type();
    case isMultiRow:
      return model->isMultiRow();
    case keyModel:
      QObject* modelPtr =
          static_cast<QObject*>(m_viewModels.at(index.row()).data());
      QQmlEngine::setObjectOwnership(modelPtr, QQmlEngine::CppOwnership);

      return QVariant::fromValue(modelPtr);
  }

  return QVariant();
}

QHash<int, QByteArray> ValueEditor::TabsModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[keyIndex] = "keyIndex";
  roles[keyNameRole] = "keyName";
  roles[keyDisplayName] = "keyTitle";
  roles[keyTTL] = "keyTtl";
  roles[keyType] = "keyType";
  roles[isMultiRow] = "isMultiRow";
  roles[keyModel] = "keyViewModel";
  return roles;
}

void ValueEditor::TabsModel::closeTab(int i) {
  if (!isIndexValid(index(i, 0))) return;

  beginRemoveRows(QModelIndex(), i, i);
  auto model = m_viewModels[i];
  m_viewModels.removeAt(i);
  endRemoveRows();

  model.clear();
}

void ValueEditor::TabsModel::setCurrentTab(int i) { m_currentTabIndex = i; }

bool ValueEditor::TabsModel::isIndexValid(const QModelIndex& index) const {
  return 0 <= index.row() && index.row() < rowCount();
}

void ValueEditor::TabsModel::tabChanged(
    QSharedPointer<ValueEditor::ValueViewModel> m) {
  int modelIndex = m_viewModels.lastIndexOf(m);

  if (modelIndex == -1) return;

  emit dataChanged(index(modelIndex, 0), index(modelIndex, 0));
}

void ValueEditor::TabsModel::tabRemoved(
    QSharedPointer<ValueEditor::ValueViewModel> m) {
  int modelIndex = m_viewModels.lastIndexOf(m);

  if (modelIndex == -1) return;

  beginRemoveRows(QModelIndex(), modelIndex, modelIndex);
  auto oldModel = m_viewModels[m_currentTabIndex];
  m_viewModels.removeAt(modelIndex);
  endRemoveRows();
  oldModel.clear();
}

void ValueEditor::TabsModel::loadModel(
    QSharedPointer<ValueEditor::Model> model,
    QWeakPointer<ConnectionsTree::KeyItem> key, bool openNewTab) {
  auto viewModel = QSharedPointer<ValueViewModel>(new ValueViewModel(model),
                                                  &QObject::deleteLater);

  if (openNewTab || m_viewModels.count() == 0) {
    beginInsertRows(QModelIndex(), m_viewModels.count(), m_viewModels.count());
    m_viewModels.append(viewModel);
    endInsertRows();
  } else {
    emit layoutAboutToBeChanged();
    auto oldModel = m_viewModels[m_currentTabIndex];
    m_viewModels.replace(m_currentTabIndex, viewModel);
    emit layoutChanged();
    emit replaceTab(m_currentTabIndex);
    oldModel.clear();
  }

  connect(viewModel.data(), &ValueViewModel::keyRenamed, this,
          [this, viewModel, key] {
            tabChanged(viewModel);
            if (key && viewModel->model())
              key.toStrongRef()->setFullPath(
                  viewModel->model()->getKeyName().toUtf8());
          });

  connect(viewModel.data(), &ValueViewModel::keyTTLChanged, this,
          [this, viewModel] { tabChanged(viewModel); });

  connect(viewModel.data(), &ValueViewModel::keyRemoved, this,
          [this, viewModel, key] {
            tabRemoved(viewModel);

            if (key) key.toStrongRef()->setRemoved();
          });
}
