#ifndef KEYITEM_H
#define KEYITEM_H

#include "treeitem.h"
#include "connections-tree/operations.h"

namespace ConnectionsTree {

    class KeyItem : public TreeItem
    {        
    public:
        KeyItem(const QByteArray& fullPath,
                unsigned short int dbIndex,
                QSharedPointer<Operations> operations,
                QWeakPointer<TreeItem> parent,
                Model &model);

        QString getDisplayName() const override;
        QIcon getIcon() const override;
        QList<QSharedPointer<TreeItem>> getAllChilds() const override;
        bool supportChildItems() const override;
        uint childCount(bool recursive = false) const override;
        QSharedPointer<TreeItem> child(uint row) const override;
        QWeakPointer<TreeItem> parent() const override;

        bool onClick(ParentView& treeView) override;
        void onWheelClick(ParentView& treeView) override;
        QSharedPointer<QMenu> getContextMenu(ParentView& treeView) override;

        bool isLocked() const override;
        bool isEnabled() const;

        QByteArray getFullPath() const;
        int getDbIndex() const;

        void setRemoved();

    private:
        QByteArray m_fullPath;
        unsigned short int m_dbIndex;
        QSharedPointer<Operations> m_operations;
        QWeakPointer<TreeItem> m_parent;
        QSharedPointer<QObject> m_signalReciever;
        bool m_removed;
    };

}
#endif // KEYITEM_H
