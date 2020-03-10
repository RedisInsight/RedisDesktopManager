#include "tabsmodel.h"
#include <qredisclient/connection.h>
#include <qredisclient/utils/text.h>
#include <QCoreApplication>
#include <QDebug>
#include <QQmlEngine>
#include <QtConcurrent>
#include "app/events.h"
#include "connections-tree/items/keyitem.h"
#include "value-editor/valueviewmodel.h"


#define TAB_NAME_LIMIT 30

ValueEditor::TabsModel::TabsModel(QSharedPointer<AbstractKeyFactory> keyFactory,
                                  QSharedPointer<Events> events)
    : m_keyFactory(keyFactory), m_events(events), m_currentTabIndex(0) {}

ValueEditor::TabsModel::~TabsModel() { m_viewModels.clear(); }

void ValueEditor::TabsModel::openTab(
    QSharedPointer<RedisClient::Connection> connection,
    QSharedPointer<ConnectionsTree::KeyItem> key, bool inNewTab) {
  auto viewModel = loadModel(
      QString(QCoreApplication::translate("RDM", "Loading key: %1 from db %2"))
          .arg(QString::fromUtf8(key->getFullPath()))
          .arg(key->getDbIndex()),
      key.toWeakRef(), inNewTab);

  auto viewModelWeekRef = viewModel.toWeakRef();

  auto loadingHandler = [this, viewModelWeekRef](QSharedPointer<Model> keyModel,
                                                 const QString& error) {
    if (keyModel.isNull() || !error.isEmpty()) {
      emit tabError(-1, QString("<b>%1</b>:\n%2")
                            .arg(QCoreApplication::translate(
                                "RDM", "Cannot open value tab"))
                            .arg(error));
      return;
    }

    auto viewModel = viewModelWeekRef.toStrongRef();

    if (viewModel) {
      viewModel->setModel(keyModel);
    }
  };

  auto callbackWrapper = [loadingHandler](QSharedPointer<Model> keyModel,
                                          const QString& error) {
    QTimer::singleShot(1, [=]() { loadingHandler(keyModel, error); });
  };

  auto conn = connection->clone();
  conn->disableAutoConnect();
  m_events->registerLoggerForConnection(*conn);

  QObject::connect(viewModel.data(), &ValueViewModel::tabClosed, [conn]() {
    if (conn) {
      conn->disconnect();
    }
  });

  try {
    QtConcurrent::run([this, conn, key, viewModelWeekRef, callbackWrapper]() {
      conn->connect();
      m_keyFactory->loadKey(conn, key->getFullPath(), key->getDbIndex(),
                            callbackWrapper);
    });
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

    if (!model) continue;

    bool tabMatch =
        (model->getConnection()->getConfig().id() ==
             connection->getConfig().id() &&
         model->dbIndex() == dbIndex && model->getKeyName().contains(filter));

    if (tabMatch) {
      beginRemoveRows(QModelIndex(), index, index);
      auto model = m_viewModels[index];
      m_viewModels.removeAt(index);
      endRemoveRows();
      index--;
      model.clear();
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

  if (!model) {
    switch (role) {
      case keyIndex:
        return index.row();
      case showLoader:
        return true;
      case tabName:
        return m_viewModels.at(index.row())->tabLoadingTitle();
    }
    return QVariant();
  }

  switch (role) {
    case keyIndex:
      return index.row();
    case keyNameRole:
      return model->getKeyName();
    case tabName:
      return model->getKeyTitle(TAB_NAME_LIMIT);
    case keyTTL:
      return model->getTTL();
    case keyType:
      return model->type();
    case rowsCount:
      return (qlonglong)model->rowsCount();
    case isMultiRow:
      return model->isMultiRow();
    case showLoader:
      return false;
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
  roles[keyTTL] = "keyTtl";
  roles[keyType] = "keyType";
  roles[isMultiRow] = "isMultiRow";
  roles[rowsCount] = "keyRowsCount";
  roles[keyModel] = "keyViewModel";
  roles[showLoader] = "showLoader";
  roles[tabName] = "tabName";
  return roles;
}

void ValueEditor::TabsModel::closeTab(int i) {
  if (!isIndexValid(index(i, 0))) return;

  beginRemoveRows(QModelIndex(), i, i);
  auto model = m_viewModels[i];
  m_viewModels.removeAt(i);
  endRemoveRows();

  model->close();
  model.clear();
}

void ValueEditor::TabsModel::setCurrentTab(int i) {
    if (0 <= i && i < rowCount()) {
        m_currentTabIndex = i;
    }
}

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
  auto oldModel = m_viewModels[modelIndex];
  m_viewModels.removeAt(modelIndex);
  endRemoveRows();
  oldModel->close();
  oldModel.clear();
}

QSharedPointer<ValueEditor::ValueViewModel> ValueEditor::TabsModel::loadModel(
    const QString& loadingBanner, QWeakPointer<ConnectionsTree::KeyItem> key,
    bool openNewTab) {
  auto viewModel = QSharedPointer<ValueViewModel>(
      new ValueViewModel(loadingBanner), &QObject::deleteLater);

  connect(viewModel.data(), &ValueViewModel::rowsLoaded, this,
          [this, viewModel](int, int) {
            qDebug() << "row loaded (tab model)";
            tabChanged(viewModel);
          });

  connect(viewModel.data(), &ValueViewModel::modelLoaded, this,
          [this, viewModel]() {
            qDebug() << "model loaded (tab model)";
            tabChanged(viewModel);
          });

  if (openNewTab || m_viewModels.count() == 0) {
    beginInsertRows(QModelIndex(), m_viewModels.count(), m_viewModels.count());
    m_viewModels.append(viewModel);
    endInsertRows();
  } else {
    emit layoutAboutToBeChanged();

    if (!(0 <= m_currentTabIndex && m_currentTabIndex < rowCount())) {
        m_currentTabIndex = rowCount() - 1;
    }

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

  return viewModel;
}
