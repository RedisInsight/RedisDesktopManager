#pragma once
#include <qredisclient/connection.h>
#include <QRegExp>
#include <QSharedPointer>
#include <QString>
#include <QtConcurrent>

#include "memoryusage.h"
#include "treeitem.h"
#include "modules/common/callbackwithowner.h"

namespace ConnectionsTree {

class Operations;
class AbstractNamespaceItem;
class Model;
class KeyItem;

class AbstractNamespaceItem : public QObject, public TreeItem, public MemoryUsage {

 public:
  AbstractNamespaceItem(Model& model, QWeakPointer<TreeItem> parent,
                        QSharedPointer<Operations> operations, uint dbIndex,
                        QRegExp filter = QRegExp());

  virtual ~AbstractNamespaceItem() {}

  QList<QSharedPointer<TreeItem>> getAllChilds() const override;

  QList<QSharedPointer<AbstractNamespaceItem>> getAllChildNamespaces() const;

  uint childCount(bool recursive = false) const override;

  uint keysCount() const;

  uint keysRenderingLimit() const;

  bool keysShortNameRendering() const;

  QSharedPointer<TreeItem> child(uint row) override;

  QWeakPointer<TreeItem> parent() const override;

  virtual void append(QSharedPointer<TreeItem> item, bool notifyModel=true);

  virtual void insertChild(QSharedPointer<TreeItem> item);

  virtual void appendKeyToIndex(QSharedPointer<KeyItem> key);

  virtual void removeNamespacedKeysFromIndex(QByteArray nsPrefix);

  virtual QHash<QByteArray, QWeakPointer<KeyItem>> getKeysIndex() const;

  virtual void removeObsoleteKeys(QList<QWeakPointer<KeyItem>> keys);

  void removeChild(int index) override;

  virtual void appendRawKey(const QByteArray& k);

  virtual void appendNamespace(QSharedPointer<AbstractNamespaceItem> item);

  virtual QSharedPointer<AbstractNamespaceItem> findChildNamespace(
      const QByteArray& name) {
    if (!m_childNamespaces.contains(name))
      return QSharedPointer<AbstractNamespaceItem>();

    return m_childNamespaces[name];
  }

  virtual uint getDbIndex() { return m_dbIndex; }

  virtual QSharedPointer<Operations> operations() { return m_operations; }

  virtual QRegExp getFilter() const { return m_filter; }

  virtual void showLoadingError(const QString& err);

  void cancelCurrentOperation() override;

  void getMemoryUsage(std::function<void(qlonglong)>) override; 

  void fetchMore() override;

 protected:
  virtual void clear();

  virtual void ensureLoaderIsCreated();

  virtual void clearLoader();

  void sortChilds();

  using RenderRawKeysCallback = CallbackWithOwner<TreeItem>;

  void renderRawKeys(const RedisClient::Connection::RawKeysList& keylist, QRegExp filter,
                     QSharedPointer<RenderRawKeysCallback> callback,
                     bool appendNewItems,
                     bool checkPreRenderedItems,
                     int maxChildItems=-1);  

  QHash<QString, std::function<bool()>> eventHandlers() override;

  void calculateUsedMemory(QSharedPointer<AsyncFuture::Deferred<qlonglong>> parentD, std::function<void(qlonglong)> callback);

  void restoreOpenedNamespaces(QSharedPointer<AbstractNamespaceItem> ns);

 protected:
  QWeakPointer<TreeItem> m_parent;
  QSharedPointer<Operations> m_operations;
  QList<QSharedPointer<TreeItem>> m_childItems;
  QHash<QByteArray, QSharedPointer<AbstractNamespaceItem>> m_childNamespaces;
  QList<QByteArray> m_rawChildKeys;
  QRegExp m_filter;  
  uint m_dbIndex;
  QSharedPointer<AsyncFuture::Deferred<qlonglong>> m_runningOperation;
  bool m_showNsOnTop;  
  QHash<QByteArray, QWeakPointer<KeyItem>> m_keysIndex;  
};
}  // namespace ConnectionsTree
