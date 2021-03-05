#pragma once
#include <qredisclient/connection.h>
#include <QRegExp>
#include <QSharedPointer>
#include <QString>
#include <QtConcurrent>

#include "memoryusage.h"
#include "treeitem.h"

namespace ConnectionsTree {

class Operations;
class AbstractNamespaceItem;
class Model;

class AbstractNamespaceItem : public QObject, public TreeItem, public MemoryUsage {

 public:
  AbstractNamespaceItem(Model& model, QWeakPointer<TreeItem> parent,
                        QSharedPointer<Operations> operations, uint dbIndex,
                        QRegExp filter = QRegExp());

  virtual ~AbstractNamespaceItem() {}

  QList<QSharedPointer<TreeItem>> getAllChilds() const override;

  QList<QSharedPointer<AbstractNamespaceItem>> getAllChildNamespaces() const;

  uint childCount(bool recursive = false) const override;

  QSharedPointer<TreeItem> child(uint row) const override;

  QWeakPointer<TreeItem> parent() const override;

  virtual void append(QSharedPointer<TreeItem> item) {
    m_childItems.append(item);
  }

  virtual void appendRawKey(const QByteArray& k) {
    m_rawChildKeys.append(k);
  }

  virtual void appendNamespace(QSharedPointer<AbstractNamespaceItem> item);

  virtual QSharedPointer<AbstractNamespaceItem> findChildNamespace(
      const QByteArray& name) {
    if (!m_childNamespaces.contains(name))
      return QSharedPointer<AbstractNamespaceItem>();

    return m_childNamespaces[name];
  }

  virtual void notifyModel();

  virtual bool isExpanded() const override { return m_expanded; }

  virtual void setExpanded(bool v) { m_expanded = v; }

  virtual uint getDbIndex() { return m_dbIndex; }

  virtual QSharedPointer<Operations> operations() { return m_operations; }

  virtual QRegExp getFilter() const { return m_filter; }

  virtual void showLoadingError(const QString& err);

  void cancelCurrentOperation() override;

  void getMemoryUsage(std::function<void(qlonglong)>) override;

 protected:
  virtual void clear();

  void sortChilds();

  QHash<QString, std::function<void()>> eventHandlers() override;

  void calculateUsedMemory(QSharedPointer<AsyncFuture::Deferred<qlonglong>> parentD, std::function<void(qlonglong)> callback);

 protected:
  QWeakPointer<TreeItem> m_parent;
  QSharedPointer<Operations> m_operations;
  QList<QSharedPointer<TreeItem>> m_childItems;
  QHash<QByteArray, QSharedPointer<AbstractNamespaceItem>> m_childNamespaces;
  QList<QByteArray> m_rawChildKeys;
  QRegExp m_filter;
  bool m_expanded;
  uint m_dbIndex;
  QSharedPointer<AsyncFuture::Deferred<qlonglong>> m_runningOperation;
  bool m_showNsOnTop;
};
}  // namespace ConnectionsTree
