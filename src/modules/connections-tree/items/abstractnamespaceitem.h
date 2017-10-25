#pragma once
#include <QRegExp>
#include <QString>
#include <QSharedPointer>
#include <QtConcurrent>
#include <qredisclient/connection.h>

#include "treeitem.h"

namespace ConnectionsTree {

    class Operations;
    class AbstractNamespaceItem;
    class Model;

    class AbstractNamespaceItem : public TreeItem
    {
    public:
        AbstractNamespaceItem(Model& model,
                              QWeakPointer<TreeItem> parent,
                              QSharedPointer<Operations> operations,
                              uint dbIndex);

        virtual ~AbstractNamespaceItem() {}

        QList<QSharedPointer<TreeItem>> getAllChilds() const override;

        uint childCount(bool recursive = false) const override;

        QSharedPointer<TreeItem> child(uint row) const override;

        QWeakPointer<TreeItem> parent() const override;

        virtual void append(QSharedPointer<TreeItem> item)
        {
            m_childItems.append(item);
        }

        virtual void appendNamespace(QSharedPointer<AbstractNamespaceItem> item)
        {
             m_childNamespaces[item->getName()] = item;
             m_childItems.append(item.staticCast<TreeItem>());
        }

        virtual QSharedPointer<AbstractNamespaceItem> findChildNamespace(const QByteArray& name)
        {
            if (!m_childNamespaces.contains(name))
                return QSharedPointer<AbstractNamespaceItem>();

            return m_childNamespaces[name];
        }

        virtual void clear();

        virtual void notifyModel();

        virtual QByteArray getName() const = 0;

        virtual QByteArray getFullPath() const = 0;

        virtual bool isExpanded() const override { return m_expanded; }

        virtual void setExpanded(bool v) { m_expanded = v; }

        virtual uint getDbIndex() { return m_dbIndex; }

    protected:
        void showLoadingError(const QString& err);

    protected:        
        QWeakPointer<TreeItem> m_parent;
        QSharedPointer<Operations> m_operations;
        QList<QSharedPointer<TreeItem>> m_childItems;
        QHash<QByteArray, QSharedPointer<AbstractNamespaceItem>> m_childNamespaces;        
        QRegExp m_filter;        
        bool m_expanded;
        uint m_dbIndex;
    };
}
