#pragma once

#include "connections-tree/operations.h"
#include "memoryusage.h"
#include "treeitem.h"

namespace ConnectionsTree {

class KeyItem : public TreeItem, public MemoryUsage {
 public:
  KeyItem(const QByteArray& fullPath, QWeakPointer<TreeItem> parent,
          Model& model);

  QString getDisplayName() const override;

  QByteArray getName() const override;

  QString type() const override { return "key"; }

  QList<QSharedPointer<TreeItem>> getAllChilds() const override;

  bool supportChildItems() const override;

  uint childCount(bool recursive = false) const override;

  QSharedPointer<TreeItem> child(uint) const override;

  QWeakPointer<TreeItem> parent() const override;

  bool isEnabled() const override;

  QByteArray getFullPath() const override;

  int getDbIndex() const;

  void setRemoved();

  void setFullPath(const QByteArray& p);

  void getMemoryUsage(std::function<void(qlonglong)> callback) override;

 protected:
  QHash<QString, std::function<void()>> eventHandlers() override;

 private:
  QByteArray m_fullPath;
  QWeakPointer<TreeItem> m_parent;
  bool m_removed;
  bool m_shortRendering;
};

}  // namespace ConnectionsTree
