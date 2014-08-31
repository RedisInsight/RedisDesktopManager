#ifndef KEYITEM_H
#define KEYITEM_H

#include "treeitem.h"
#include "connections-tree/operations.h"

namespace ConnectionsTree {

    class KeyItem : public TreeItem
    {
    public:
        KeyItem(const QString& fullPath, QSharedPointer<Operations> operations, const TreeItem* parent);

        QString getDisplayName() const override;
        QIcon getIcon() const override;
        QList<QSharedPointer<TreeItem>> getAllChilds() const override;
        uint childCount() const override;
        QSharedPointer<TreeItem> child(int row) const override;
        const TreeItem* parent() const override;

        bool onClick(ParentView& treeView) override;
        void onWheelClick(ParentView& treeView) override;
        QSharedPointer<QMenu> getContextMenu(ParentView& treeView) override;

        bool isLocked() const override;
        bool isEnabled() const;

    private:
        QString m_fullPath;
        QSharedPointer<Operations> m_operations;
        const TreeItem* m_parent;
        bool m_locked;
        bool m_removed;

    };

}
#endif // KEYITEM_H
