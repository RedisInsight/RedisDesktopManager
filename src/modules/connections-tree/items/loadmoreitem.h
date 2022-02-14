#pragma once

#include "connections-tree/operations.h"
#include "treeitem.h"

namespace ConnectionsTree {

class LoadMoreItem : public TreeItem {
 public:
  LoadMoreItem(QWeakPointer<TreeItem> parent, Model& model);

  QString getDisplayName() const override;

  QString type() const override { return "loader"; }

  QList<QSharedPointer<TreeItem>> getAllChilds() const override;

  bool supportChildItems() const override;

  uint childCount(bool recursive = false) const override;

  QSharedPointer<TreeItem> child(uint) override;

  QWeakPointer<TreeItem> parent() const override;

  bool isEnabled() const override;

 protected:
  QHash<QString, std::function<bool()>> eventHandlers() override;

 private:
  QWeakPointer<TreeItem> m_parent;
};

}  // namespace ConnectionsTree
