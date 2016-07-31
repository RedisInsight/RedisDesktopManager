#pragma once
#include <QAbstractItemModel>
#include <QList>
#include <QVariant>
#include <QSharedPointer>

namespace ConnectionsTree {

    class TreeItem;
    class ServerItem;

    class Model : public QAbstractItemModel
    {
        Q_OBJECT
    public:
        explicit Model(QObject *parent = 0);

        QVariant data(const QModelIndex &index, int role) const;
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

        QModelIndex getIndexFromItem(QWeakPointer<TreeItem>);

        bool canFetchMore(const QModelIndex &parent) const;

        void fetchMore(const QModelIndex &parent);

    signals:
        void error(const QString& err);
        void itemChanged(QWeakPointer<TreeItem> item);
        void itemChildsLoaded(QWeakPointer<TreeItem> item);
        void itemChildsUnloaded(QWeakPointer<TreeItem> item);

    protected slots:
        void onItemChanged(QWeakPointer<TreeItem>);
        void onItemChildsLoaded(QWeakPointer<TreeItem> item);
        void onItemChildsUnloaded(QWeakPointer<TreeItem> item);

    protected:            
        void addRootItem(QSharedPointer<ServerItem> item);
        void removeRootItem(QSharedPointer<ServerItem> item);        
    private:
         QList<QSharedPointer<TreeItem>> m_treeItems;
         QSharedPointer<QHash<TreeItem*, QWeakPointer<TreeItem>>> m_rawPointers;
    };
}


