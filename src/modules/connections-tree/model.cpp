#include "model.h"
#include <QDebug>
#include <QSettings>
#include <QWeakPointer>
#include <algorithm>
#include "items/serveritem.h"
#include "items/servergroup.h"
#include "items/databaseitem.h"

using namespace ConnectionsTree;

Model::Model(QObject *parent)
    : QAbstractItemModel(parent),
      m_rawPointers(new QHash<TreeItem *, QWeakPointer<TreeItem>>()) {
  QObject::connect(this, &Model::itemChanged, this, &Model::onItemChanged);
  QObject::connect(this, &Model::itemChildsLoaded, this,
                   &Model::onItemChildsLoaded);
  QObject::connect(this, &Model::itemChildsUnloaded, this,
                   &Model::onItemChildsUnloaded);
  QObject::connect(this, &Model::expandItem, this, &Model::onExpandItem);

  QObject::connect(this, &Model::beforeItemLayoutChanged, this,
                   &Model::onBeforeItemLayoutChanged);
  QObject::connect(this, &Model::itemLayoutChanged, this,
                   &Model::onItemLayoutChanged);

  qRegisterMetaType<QWeakPointer<TreeItem>>("QWeakPointer<TreeItem>");
}

QVariant Model::data(const QModelIndex &index, int role) const {
  const TreeItem *item = getItemFromIndex(index);

  if (item == nullptr) return QVariant();

  switch (role) {
    case itemName:
      return item->getDisplayName();
    case itemType:
      return item->type();
    case itemIsInitiallyExpanded:
      return item->isExpanded();
    case itemMetaData:
      return item->metadata();
  }

  return QVariant();
}

QHash<int, QByteArray> Model::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[itemName] = "name";
  roles[itemIsInitiallyExpanded] = "expanded";
  roles[itemMetaData] = "metadata";
  return roles;
}

Qt::ItemFlags Model::flags(const QModelIndex &index) const {
  const TreeItem *item = getItemFromIndex(index);

  if (item == nullptr) return Qt::NoItemFlags;

  Qt::ItemFlags result = Qt::ItemIsSelectable;

  if (item->isEnabled()) result |= Qt::ItemIsEnabled;

  return result;
}

QModelIndex Model::index(int row, int column, const QModelIndex &parent) const {
  if (!hasIndex(row, column, parent)) return QModelIndex();

  const TreeItem *parentItem = getItemFromIndex(parent);
  QSharedPointer<TreeItem> childItem;

  // get item from root items
  if (parentItem) {
    childItem = parentItem->child(row);
  } else if (row < m_treeItems.size()) {
    childItem = m_treeItems.at(row);
  }

  if (childItem.isNull())
    return QModelIndex();
  else {
    m_rawPointers->insert(childItem.data(), childItem.toWeakRef());
    return createIndex(row, column, childItem.data());
  }
}

QModelIndex Model::parent(const QModelIndex &index) const {
  const TreeItem *childItem = getItemFromIndex(index);

  if (!childItem) return QModelIndex();

  QWeakPointer<TreeItem> parentItem = childItem->parent();

  if (!parentItem) return QModelIndex();

  m_rawPointers->insert(parentItem.data(), parentItem);
  return createIndex(parentItem.toStrongRef()->row(), 0,
                     (void *)parentItem.data());
}

int Model::rowCount(const QModelIndex &parent) const {
  const TreeItem *parentItem = getItemFromIndex(parent);

  if (!parentItem) return m_treeItems.size();

  return parentItem->childCount();
}

bool Model::hasChildren(const QModelIndex &parent) {
  const TreeItem *parentItem = getItemFromIndex(parent);

  if (!parentItem) return m_treeItems.size() > 0;

  if (parentItem->type() == "key") return false;

  if (parentItem->type() == "namespace"
          || parentItem->type() == "server"
          || parentItem->type() == "server_group")
    return true;

  return parentItem->childCount() > 0;
}

QModelIndex Model::getIndexFromItem(QWeakPointer<TreeItem> item) {
  if (!item) {
    return QModelIndex();
  }

  auto sRef = item.toStrongRef();

  if (!sRef) {
    return QModelIndex();
  }

  if (!sRef->parent()) {
    return index(sRef->row(), 0, QModelIndex());
  }

  return createIndex(item.toStrongRef()->row(), 0, (void *)item.data());
}

bool Model::canFetchMore(const QModelIndex &parent) const {
  TreeItem *i = getItemFromIndex(parent);

  return i && i->canFetchMore();
}

void Model::fetchMore(const QModelIndex &parent) {
  TreeItem *i = getItemFromIndex(parent);

  if (!i) return;

  i->fetchMore();
}

void Model::onItemChanged(QWeakPointer<TreeItem> item) {
  if (!item) return;

  auto index = getIndexFromItem(item);

  if (!index.isValid()) return;  

  emit dataChanged(index, index);
}

void Model::onItemChildsLoaded(QWeakPointer<TreeItem> item) {
  if (!item) return;

  auto index = getIndexFromItem(item);

  if (!index.isValid()) return;

  QSharedPointer<TreeItem> treeItem = item.toStrongRef();

  if (!treeItem->childCount()) {
    return;
  }

  emit beginInsertRows(index, 0, treeItem->childCount() - 1);
  emit endInsertRows();

  emit dataChanged(index, index);

  if (treeItem->type() == "database") {
    emit expand(index);

    QSettings settings;
    m_expanded.clear();

    if (settings.value("app/reopenNamespacesOnReload", true).toBool()) {     
      restoreOpenedNamespaces(treeItem.staticCast<AbstractNamespaceItem>());
    }
  } else if (treeItem->type() == "server"
             || treeItem->type() == "server_group"
             || treeItem->type() == "namespace") {
    emit expand(index);
    emit dataChanged(index, index);
  }
}

void Model::onItemChildsUnloaded(QWeakPointer<TreeItem> item) {
  if (!item) return;

  auto index = getIndexFromItem(item);

  if (!index.isValid()) return;

  if (item.toStrongRef()->childCount() == 0)
      return;

  emit beginRemoveRows(index, 0, item.toStrongRef()->childCount() - 1);
  emit endRemoveRows();
}

void Model::onExpandItem(QWeakPointer<TreeItem> item) {
  if (!item) return;

  auto index = getIndexFromItem(item);

  if (!index.isValid()) return;

  emit expand(index);
}

void Model::onBeforeItemLayoutChanged(QWeakPointer<TreeItem> item) {
  if (!item) return;

  auto itemS = item.toStrongRef();

  auto index = getIndexFromItem(item);

  if (!index.isValid()) return;

  emit layoutAboutToBeChanged({index}, QAbstractItemModel::VerticalSortHint);

  m_pendingChanges.clear();

  for (long rowIndex = 0; rowIndex < itemS->childCount(); rowIndex++) {
    auto child = itemS->child(rowIndex);
    m_pendingChanges.insert(child, getIndexFromItem(child));
  }
}

void Model::onItemLayoutChanged(QWeakPointer<TreeItem> item) {
  if (!item) return;

  auto itemS = item.toStrongRef();

  auto index = getIndexFromItem(item);

  if (!index.isValid()) return;

  for (long rowIndex = 0; rowIndex < itemS->childCount(); rowIndex++) {
    auto child = itemS->child(rowIndex);

    if (!m_pendingChanges.contains(child)) continue;

    changePersistentIndex(m_pendingChanges.take(child),
                          getIndexFromItem(child));
  }

  m_pendingChanges.clear();

  emit layoutChanged({}, QAbstractItemModel::VerticalSortHint);

  for (long rowIndex = 0; rowIndex < itemS->childCount(); rowIndex++) {
    auto child = itemS->child(rowIndex);
    auto childIndex = getIndexFromItem(child);

    emit dataChanged(childIndex, childIndex);
  }
}

QVariant Model::getMetadata(const QModelIndex &index, const QString &metaKey) {
  TreeItem *item = getItemFromIndex(index);

  if (item == nullptr) return QVariant();

  return item->metadata(metaKey);
}

void Model::setMetadata(const QModelIndex &index, const QString &metaKey,
                        QVariant value) {
  TreeItem *item = getItemFromIndex(index);

  if (item == nullptr) return;

  item->setMetadata(metaKey, value);
}

void Model::sendEvent(const QModelIndex &index, QString event) {  
  TreeItem *item = getItemFromIndex(index);

  if (!item)
      return;

  item->handleEvent(event);
}

unsigned int Model::size() { return m_treeItems.size(); }

void Model::setExpanded(const QModelIndex &index) {
  TreeItem *item = getItemFromIndex(index);

  if (!item || item->type() != "namespace") return;

  m_expanded.insert(item->getFullPath());
}

void Model::setCollapsed(const QModelIndex &index) {
  TreeItem *item = getItemFromIndex(index);

  if (!item || item->type() != "namespace") return;

  // TODO: remove child ns

  m_expanded.remove(item->getFullPath());
}

void Model::collapseRootItems()
{
    for (auto item : m_treeItems) {

        auto server = item.dynamicCast<SortableTreeItem>();

        if (!server)
            continue;

        server->unload();
    }
}

void Model::dropItemAt(const QModelIndex &index, const QModelIndex &at)
{
    if (!(index.isValid() && at.isValid()))
        return;

    auto item = getItemFromIndex(index);
    auto targetItem = getItemFromIndex(at);

    if (!(item && targetItem)) return;

    if (!(item->type() == "server"
          && targetItem->type() == "server_group"
          && (!item->parent() || item->parent() != targetItem->getSelf()))) {        
        return;
    }      

    int targetIndex = targetItem->childCount();

    auto srcParent = QModelIndex();
    auto targetParent = at;

    if (item->parent()) {
        srcParent = getIndexFromItem(item->parent());
    }

    bool res = beginMoveRows(srcParent, index.row(), index.row(),
                             targetParent, targetIndex);

    if (!res) {        
        return;
    }    

    auto findRootItem = [](TreeItem* t, QList<QSharedPointer<TreeItem>> treeItems) {
        for (auto rI : treeItems) {
            if (t == rI.data())
                return rI;
        }

        return QSharedPointer<TreeItem>();
    };

    QSharedPointer<TreeItem> srv;

    if (item->parent()) {
        srv = findRootItem(item, item->parent().toStrongRef()->getAllChilds());

        auto sourceGroup = item->parent().toStrongRef().dynamicCast<ServerGroup>();

        if (!sourceGroup) {
            qDebug() << "invalid source group";
            return;
        }

        sourceGroup->removeChild(srv);
    } else {
      srv = findRootItem(item, m_treeItems);      
      m_treeItems.removeAll(srv);      
    }

    endMoveRows();

    auto targetGroup = findRootItem(targetItem, m_treeItems).dynamicCast<ServerGroup>();

    if (!targetGroup) {
        qDebug() << "invalid target group";
        return;
    }

    targetGroup->addServer(srv);

    auto srvItem = srv.dynamicCast<ServerItem>();

    if (!srvItem) {
        qDebug() << "invalid srv item";
        return;
    }

    srvItem->setParent(targetGroup.toWeakRef());    

    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

void Model::applyGroupChanges()
{
    emit layoutAboutToBeChanged();        

    // TBD

    emit layoutChanged();
}

void Model::addRootItem(QSharedPointer<SortableTreeItem> item) {
  if (item.isNull()) return;

  int insertIndex = m_treeItems.size();

  emit beginInsertRows(QModelIndex(), insertIndex, insertIndex);

  item->setRow(insertIndex);

  m_treeItems.push_back(item);

  emit endInsertRows();

  if (item->isExpanded() && item->childCount() > 0) {
      QTimer::singleShot(100, this, [this, item]() {
        if (item)
            emit expand(getIndexFromItem(item));
      });
  }
}

void Model::removeRootItem(QSharedPointer<TreeItem> item) {
  if (!item) return;

  beginRemoveRows(QModelIndex(), item->row(), item->row());
  m_treeItems.removeAll(item);
  endRemoveRows();
}

void Model::restoreOpenedNamespaces(QSharedPointer<AbstractNamespaceItem> ns)
{
    if (ns->type() == "namespace" && !ns->isExpanded())
        return;

    if (ns->isExpanded())
        emit expand(getIndexFromItem(ns.staticCast<TreeItem>().toWeakRef()));

    auto childs = ns->getAllChildNamespaces();

    for (auto childNs : childs) {
        restoreOpenedNamespaces(childNs);
    }
}
