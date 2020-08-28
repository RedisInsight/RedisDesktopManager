#pragma once
#include <QDebug>
#include <QFuture>
#include <QHash>
#include <QIcon>
#include <QList>
#include <QModelIndex>
#include <QSharedPointer>
#include <QString>
#include <QVariantMap>
#include <QWeakPointer>
#include <functional>

namespace ConnectionsTree {

class Model;

class TreeItem {
 public:
  TreeItem(Model& m);

  virtual ~TreeItem() {}

  virtual QString getDisplayName() const = 0;

  virtual QByteArray getName() const { return getDisplayName().toUtf8(); }

  virtual QByteArray getFullPath() const { return QByteArray(); }

  virtual QString type() const = 0;

  virtual QList<QSharedPointer<TreeItem>> getAllChilds() const = 0;

  virtual uint childCount(bool recursive = false) const = 0;

  virtual QSharedPointer<TreeItem> child(uint row) const = 0;

  virtual QWeakPointer<TreeItem> parent() const { return QWeakPointer<TreeItem>(); }

  virtual bool supportChildItems() const { return true; }

  virtual QVariant metadata(const QString& key) const;

  virtual QVariantMap metadata() const;

  virtual void setMetadata(const QString&, QVariant) {}

  virtual int row() const;

  virtual QWeakPointer<TreeItem> getSelf();

  virtual void handleEvent(QString event);

  virtual void cancelCurrentOperation();

  virtual bool isLocked() const { return m_locked; }

  virtual bool isEnabled() const { return true; };

  virtual bool isExpanded() const { return false; }

  virtual bool canFetchMore() const { return false; }

  virtual void fetchMore() {}

  virtual Model& model();

 protected:
  void lock();
  void unlock();
  virtual QHash<QString, std::function<void()>> eventHandlers();

 protected:
  Model& m_model;  
  QWeakPointer<TreeItem> m_selfPtr;
  bool m_locked;
  QFuture<void> m_currentOperation;
};

typedef QList<QSharedPointer<TreeItem>> TreeItems;

}  // namespace ConnectionsTree
