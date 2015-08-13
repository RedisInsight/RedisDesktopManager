#pragma once
#include "treeitem.h"
#include "connections-tree/operations.h"

namespace ConnectionsTree {

    class NamespaceItem : public TreeItem
    {
    public:
        NamespaceItem(const QString& fullPath,  QSharedPointer<Operations> operations, QWeakPointer<TreeItem> parent);

        QString getDisplayName() const override;
        QIcon getIcon() const override;
        QList<QSharedPointer<TreeItem>> getAllChilds() const override;
        uint childCount() const override;
        QSharedPointer<TreeItem> child(uint row) const override;
        QWeakPointer<TreeItem> parent() const override;

        bool onClick(ParentView& treeView) override;
        QSharedPointer<QMenu> getContextMenu(ParentView& treeView) override;

        bool isLocked() const override;
        bool isEnabled() const override;

        void append(QSharedPointer<TreeItem> item);

        QSharedPointer<NamespaceItem> findChildNamespace(const QString& name);

    private:
        QString m_fullPath;
        QSharedPointer<Operations> m_operations;
        QWeakPointer<TreeItem> m_parent;
        bool m_locked;
        QList<QSharedPointer<TreeItem>> m_childItems;
        QHash<QString, QSharedPointer<NamespaceItem>> m_childNamespaces;
    };
}
