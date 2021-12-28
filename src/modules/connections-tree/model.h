#pragma once
#include <QAbstractItemModel>
#include <QDebug>
#include <QList>
#include <QQuickImageProvider>
#include <QSharedPointer>
#include <QVariant>

#include "items/sortabletreeitem.h"

namespace ConnectionsTree {

class ServerItem;
class AbstractNamespaceItem;

class Model : public QAbstractItemModel {
  Q_OBJECT
 public:
  enum Roles {
    itemName = Qt::UserRole + 1,
    itemType,
    itemIsInitiallyExpanded,
    itemMetaData,
  };

 public:
  explicit Model(QObject *parent = 0);

  QVariant data(const QModelIndex &index, int role) const override;

  QHash<int, QByteArray> roleNames() const override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  QModelIndex index(int row, int column, const QModelIndex &parent) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

  inline int columnCount(const QModelIndex &parent = QModelIndex()) const override {
    Q_UNUSED(parent);
    return 1;
  }

  inline TreeItem *getItemFromIndex(const QModelIndex &index) const {
    if (!index.isValid()) return nullptr;
    if (index.model() != this) return nullptr;

    TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
    if (!item || !m_rawPointers->contains(item)) return nullptr;

    if (!m_rawPointers->value(item)) {
      m_rawPointers->remove(item);
      return nullptr;
    }
    return item;
  }

  QModelIndex getIndexFromItem(QWeakPointer<TreeItem>);

  QSet<QByteArray> expandedNamespaces;

 signals:
  void expand(const QModelIndex &index);

  void error(const QString &err);

  void itemChanged(QWeakPointer<TreeItem> item);

 public:
  void beforeItemChildsUnloaded(QWeakPointer<TreeItem> item);  

  void beforeChildLoadedAtPos(QWeakPointer<TreeItem> item, int pos);

  void beforeChildLoaded(QWeakPointer<TreeItem> item, int count);

  void childLoaded(QWeakPointer<TreeItem> item);

  void beforeItemChildRemoved(QWeakPointer<TreeItem> item, int row);

  void itemChildRemoved(QWeakPointer<TreeItem> childItem);

  void expandItem(QWeakPointer<TreeItem> item);

  void beforeItemLayoutChanged(QWeakPointer<TreeItem> item);

  void itemLayoutChanged(QWeakPointer<TreeItem> item);

 public slots:
  void onItemChanged(QWeakPointer<TreeItem> item);

  void setMetadata(const QModelIndex &index, const QString &metaKey,
                   QVariant value);

  void sendEvent(const QModelIndex &index, QString event);

  virtual int size();

  void setExpanded(const QModelIndex &index);

  void setCollapsed(const QModelIndex &index);

  void collapseRootItems();

  void dropItemAt(const QModelIndex &index, const QModelIndex &at);

  virtual void applyGroupChanges();

 protected:
  void addRootItem(QSharedPointer<ConnectionsTree::SortableTreeItem> item);

  void removeRootItem(QSharedPointer<TreeItem> item);

  typedef QPair<QWeakPointer<TreeItem>, QModelIndex> PendingIndexChange;

  void iterateAllChilds(QSharedPointer<TreeItem> item, QList<PendingIndexChange> &pendingChanges);  

 protected:
  QList<QSharedPointer<TreeItem>> m_treeItems;
  QSharedPointer<QHash<TreeItem *, QWeakPointer<TreeItem>>> m_rawPointers; 
  QHash<QSharedPointer<TreeItem>, QList<PendingIndexChange>> m_pendingChanges;
};
}  // namespace ConnectionsTree
