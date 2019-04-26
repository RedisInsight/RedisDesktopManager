#pragma once

#include "connections-tree/operations.h"
#include "treeitem.h"

namespace ConnectionsTree {

class KeyItem : public TreeItem {
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

  bool isEnabled() const;

  QByteArray getFullPath() const;

  int getDbIndex() const;

  void setRemoved();

  void setFullPath(const QByteArray& p);

 protected:
  QHash<QString, std::function<void()>> eventHandlers() override;

 private:
  QByteArray m_fullPath;
  QWeakPointer<TreeItem> m_parent;
  bool m_removed;
};

}  // namespace ConnectionsTree
