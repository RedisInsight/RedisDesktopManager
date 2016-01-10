#pragma once
#include <QAbstractItemModel>
#include <QList>
#include <QVariant>
#include <QSharedPointer>
#include <QQuickImageProvider>
#include <QDebug>

#include "items/treeitem.h"

namespace ConnectionsTree {

    class ServerItem;

    class Model : public QAbstractItemModel
    {
        Q_OBJECT
    public:
        enum Roles {
            itemName = Qt::UserRole + 1,
            itemType,
            itemMeta,
        };

    public:
        explicit Model(QObject *parent = 0);

        QVariant data(const QModelIndex &index, int role) const;

        QHash<int, QByteArray> roleNames() const override;

        Qt::ItemFlags flags(const QModelIndex& index) const;

        QModelIndex index(int row, int column, const QModelIndex & parent) const;

        QModelIndex parent(const QModelIndex & index) const;

        int rowCount(const QModelIndex & parent = QModelIndex()) const;

        inline int columnCount(const QModelIndex & parent = QModelIndex()) const
        {
            Q_UNUSED(parent);
            return 1;
        }
        
        inline TreeItem *getItemFromIndex(const QModelIndex &index) const {
            if (!index.isValid())
                return nullptr;
            if (index.model() != this)
                return nullptr;
                
            TreeItem *parent = static_cast<TreeItem*>(index.internalPointer());
            if (!parent || !m_rawPointers->contains(parent))
                return nullptr;

            if (!m_rawPointers->value(parent)) {
                m_rawPointers->remove(parent);
                return nullptr;
            }

            return parent;
        }

    public slots:
        QVariant getItemIcon(const QModelIndex &index)
        {
            return data(index, Qt::DecorationRole);
        }

        QVariant getItemType(const QModelIndex &index)
        {
            return data(index, itemType);
        }

        void sendEvent(const QModelIndex &index, QString event)
        {
            qDebug() << "Event recieved:" << event;

            TreeItem * item = getItemFromIndex(index);

            if (item)
                item->handleEvent(event);
        }

        unsigned int size()
        {
            return m_treeItems.size();
        }

    protected:            
        void addRootItem(QSharedPointer<ServerItem> item);
        void removeRootItem(QSharedPointer<ServerItem> item);        
    private:
         QList<QSharedPointer<TreeItem>> m_treeItems;
         QSharedPointer<QHash<TreeItem*, QWeakPointer<TreeItem>>> m_rawPointers;
    };
}
