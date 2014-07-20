#ifndef SERVERITEM_H
#define SERVERITEM_H

#include "treeitem.h"
#include "connections-tree/operations.h"
#include <QList>

namespace ConnectionsTree {

    class ServerItem : public TreeItem
    {
    public:
        ServerItem(QSharedPointer<Operations> operations);

        QString getDisplayName() const;
        QIcon getIcon() const;
        QList<QSharedPointer<TreeItem>> getAllChilds() const;
        uint childCount() const;
        QSharedPointer<TreeItem> child(int row) const;
        QSharedPointer<TreeItem> parent() const {return QSharedPointer<TreeItem>();}

        bool onClick(QWeakPointer<TreeItem::ParentView> treeView, QWeakPointer<QTabWidget> tabs);
        void onWheelClick(QWeakPointer<ParentView> treeView, QWeakPointer<QTabWidget> tabs) {}
        QSharedPointer<QMenu> getContextMenu(QWeakPointer<TreeItem::ParentView> treeView, QWeakPointer<QTabWidget> tabs);

        bool isLocked() const;

        void load();
        void unload();
        void reload();

    private:
        bool m_locked;
        bool m_connected;
        bool m_databaseListLoaded;
        QSharedPointer<Operations> m_operations;
        QList<QSharedPointer<TreeItem>> m_databases;

        QString m_name;

    };
}

#endif // SERVERITEM_H
