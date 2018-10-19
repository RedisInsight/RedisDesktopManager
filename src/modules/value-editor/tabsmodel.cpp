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
    ConnectionsTree::KeyItem& key, bool inNewTab) {
  try {
    m_keyFactory->loadKey(
        connection, key.getFullPath(), key.getDbIndex(),
        [this, inNewTab, &key](QSharedPointer<Model> keyModel,
                               const QString& error) {
          if (keyModel.isNull() || !error.isEmpty()) {
            emit keyError(-1, QString("<b>%1</b>:\n%2")
                                  .arg(QCoreApplication::translate(
                                      "RDM", "Cannot open value tab"))
                                  .arg(error));
            return;
          }

          auto viewModel = loadModel(keyModel, inNewTab);
          auto weakKeyModel = viewModel.toWeakRef();

          connect(keyModel->getConnector().data(), &ModelSignals::removed, this,
                  [this, weakKeyModel, &key]() {
                    // NOTE(u_glide): React in 100 ms to make sure that keymodel
                    // update is finished
                    QTimer::singleShot(100, [this, weakKeyModel, &key]() {
                      auto keyModel = weakKeyModel.toStrongRef();

                      if (!keyModel) return;

                      removeModel(keyModel);
                      key.setRemoved();  // Disable key in connections tree
                    });
                  });
        });
  } catch (...) {
    emit keyError(-1, QCoreApplication::translate(
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
      return model->getType();
    case isMultiRow:
      return model->isMultiRow();
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
  return roles;
}

void ValueEditor::TabsModel::addKey(QString keyName, QString keyType,
                                    const QVariantMap& row,
                                    QJSValue jsCallback) {
  if (m_newKeyRequest.first.isNull()) {
    qDebug() << "Invalid new key request";
    return;
  }

  auto connection = m_newKeyRequest.first.toStrongRef();

  if (!connection) {
    qDebug() << "Invalid new key request";
    return;
  }

  try {
    m_keyFactory->addKey(connection, keyName.toUtf8(), m_newKeyRequest.second,
                         keyType, row);
    m_newKeyCallback();

    if (jsCallback.isCallable()) jsCallback.call(QJSValueList{});

    m_newKeyRequest = NewKeyRequest();
  } catch (const Model::Exception& e) {
    if (jsCallback.isCallable())
      jsCallback.call(QJSValueList{
          QCoreApplication::translate("RDM", "Can't add new key: ") +
          QString(e.what())});
  }
}

void ValueEditor::TabsModel::renameKey(int i, const QString& newKeyName) {
  if (!isIndexValid(index(i, 0))) return;

  auto model = m_viewModels.at(i)->model();

  try {
    model->setKeyName(printableStringToBinary(newKeyName));
    emit dataChanged(index(i, 0), index(i, 0));
  } catch (const Model::Exception& e) {
    emit keyError(i, QCoreApplication::translate("RDM", "Can't rename key: ") +
                         QString(e.what()));
  }
}

void ValueEditor::TabsModel::removeKey(int i) {
  if (!isIndexValid(index(i, 0))) return;

  auto model = m_viewModels.at(i)->model();

  try {
    model->removeKey();
  } catch (const Model::Exception& e) {
    emit keyError(i, QCoreApplication::translate("RDM", "Can't remove key: ") +
                         QString(e.what()));
  }
}

void ValueEditor::TabsModel::setTTL(int i, const QString& newTTL) {
  if (!isIndexValid(index(i, 0))) return;

  auto model = m_viewModels.at(i)->model();

  try {
    model->setTTL(newTTL.toLong());
    emit dataChanged(index(i, 0), index(i, 0));
  } catch (const Model::Exception& e) {
    emit keyError(i, QCoreApplication::translate("RDM", "Can't set key ttl: ") +
                         QString(e.what()));
  }
}

void ValueEditor::TabsModel::closeTab(int i) {
  if (!isIndexValid(index(i, 0))) return;

  try {
    beginRemoveRows(QModelIndex(), i, i);
    auto model = m_viewModels[i];
    m_viewModels.removeAt(i);
    endRemoveRows();

    model.clear();
  } catch (const Model::Exception& e) {
    emit keyError(i,
                  QCoreApplication::translate("RDM", "Can't close key tab: ") +
                      QString(e.what()));
  }
}

void ValueEditor::TabsModel::setCurrentTab(int i) { m_currentTabIndex = i; }

QObject* ValueEditor::TabsModel::getValue(int i) {
  if (!isIndexValid(index(i, 0))) return nullptr;

  QObject* modelPtr = qobject_cast<QObject*>(m_viewModels.at(i).data());
  QQmlEngine::setObjectOwnership(modelPtr, QQmlEngine::CppOwnership);
  return modelPtr;
}

void ValueEditor::TabsModel::openNewKeyDialog(
    QSharedPointer<RedisClient::Connection> connection,
    std::function<void()> callback, int dbIndex, QString keyPrefix) {
  if (connection.isNull() || dbIndex < 0) return;

  m_newKeyRequest = qMakePair(connection.toWeakRef(), dbIndex);
  m_newKeyCallback = callback;

  QString dbId =
      QString("%1:db%2").arg(connection->getConfig().name()).arg(dbIndex);

  emit newKeyDialog(dbId, keyPrefix);
}

bool ValueEditor::TabsModel::isIndexValid(const QModelIndex& index) const {
  return 0 <= index.row() && index.row() < rowCount();
}

QSharedPointer<ValueEditor::ValueViewModel> ValueEditor::TabsModel::loadModel(
    QSharedPointer<ValueEditor::Model> model, bool openNewTab) {
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
  return viewModel;
}

void ValueEditor::TabsModel::removeModel(QSharedPointer<ValueViewModel> model) {
  int i = m_viewModels.lastIndexOf(model);

  if (i == -1) {
    qDebug() << "[Remove model] Key model not found!";
    return;
  }

  beginRemoveRows(QModelIndex(), i, i);
  auto oldModel = m_viewModels[m_currentTabIndex];
  m_viewModels.removeAt(i);
  endRemoveRows();
  oldModel.clear();
}
