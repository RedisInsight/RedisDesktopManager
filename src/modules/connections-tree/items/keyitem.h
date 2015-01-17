#ifndef KEYITEM_H
#define KEYITEM_H

#include "treeitem.h"
#include "connections-tree/operations.h"

namespace ConnectionsTree {

    class KeyItem : public QObject, public TreeItem
    {
        Q_OBJECT
    public:
        KeyItem(const QString& fullPath, int dbIndex, QSharedPointer<Operations> operations, const TreeItem* parent);

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

        QString getFullPath() const;
        int getDbIndex() const;

        void setRemoved();

    private:
        QString m_fullPath;
        int m_dbIndex;
        QSharedPointer<Operations> m_operations;
        const TreeItem* m_parent;
        bool m_locked;
        bool m_removed;

    };

}
#endif // KEYITEM_H
