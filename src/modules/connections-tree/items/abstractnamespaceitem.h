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

    class KeysTreeRenderer
    {
    public:
        struct RenderingSettigns {
            QRegExp filter;
            QString nsSeparator;
            int dbIndex;
        };

    public:
        static void renderKeys(QSharedPointer<Operations> operations,
                               RedisClient::Connection::RawKeysList keys,
                               QSharedPointer<AbstractNamespaceItem> parent,
                               RenderingSettigns settings, const QSet<QByteArray> &expandedNamespaces);
    private:
        static void renderLazily(QSharedPointer<AbstractNamespaceItem> parent,
                                 const QByteArray &notProcessedKeyPart,
                                 const QByteArray &fullKey,
                                 QSharedPointer<Operations> operations,
                                 const RenderingSettigns& settings,
                                 const QSet<QByteArray> &expandedNamespaces,
                                 unsigned long level=0);
    };


    class AbstractNamespaceItem : public TreeItem
    {
    public:
        AbstractNamespaceItem(Model& model,
                              QWeakPointer<TreeItem> parent,
                              QSharedPointer<Operations> operations,
                              const KeysTreeRenderer::RenderingSettigns& rSettings=KeysTreeRenderer::RenderingSettigns());

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

        virtual void append(const QByteArray& item)
        {
            m_rawChilds.append(item);
        }

        virtual QSharedPointer<AbstractNamespaceItem> findChildNamespace(const QByteArray& name)
        {
            if (!m_childNamespaces.contains(name))
                return QSharedPointer<AbstractNamespaceItem>();

            return m_childNamespaces[name];
        }

        virtual void clear(bool removeRawKeys=true);

        virtual void notifyModel();

        virtual QByteArray getName() const = 0;

        virtual QByteArray getFullPath() const = 0;

        virtual bool isExpanded() const override { return m_expanded; }

        virtual void setExpanded(bool v) { m_expanded = v; }

    protected:
        void renderChilds();

    protected:        
        QWeakPointer<TreeItem> m_parent;
        QSharedPointer<Operations> m_operations;
        QList<QSharedPointer<TreeItem>> m_childItems;
        QHash<QByteArray, QSharedPointer<AbstractNamespaceItem>> m_childNamespaces;
        RedisClient::Connection::RawKeysList m_rawChilds;
        KeysTreeRenderer::RenderingSettigns m_renderingSettings;
        bool m_expanded;
    };
}
