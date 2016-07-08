#pragma once
#include "treeitem.h"
#include "connections-tree/operations.h"

namespace ConnectionsTree {

    class NamespaceItem : public TreeItem
    {
    public:
        NamespaceItem(const QString& fullPath,  unsigned short dbIndex,
                      QSharedPointer<Operations> operations, QWeakPointer<TreeItem> parent);

        QString getDisplayName() const override;
        QString getName() const;
        QIcon getIcon() const override;
        QList<QSharedPointer<TreeItem>> getAllChilds() const override;
        uint childCount(bool recursive = false) const override;
        QSharedPointer<TreeItem> child(uint row) const override;
        QWeakPointer<TreeItem> parent() const override;

        bool onClick(ParentView& treeView) override;
        QSharedPointer<QMenu> getContextMenu(ParentView& treeView) override;

        bool isLocked() const override;
        bool isEnabled() const override;

        void append(QSharedPointer<TreeItem> item);

        QByteArray getFullPath() const;
        int getDbIndex() const;

        void setRemoved();

        QSharedPointer<NamespaceItem> findChildNamespace(const QString& name);

    private:
        QString m_fullPath;
        unsigned short int m_dbIndex;
        QString m_displayName;
        QSharedPointer<Operations> m_operations;
        QWeakPointer<TreeItem> m_parent;        
        QList<QSharedPointer<TreeItem>> m_childItems;
        QHash<QString, QSharedPointer<NamespaceItem>> m_childNamespaces;
        QSharedPointer<QObject> m_signalReciever;
        bool m_removed;
    };
}
