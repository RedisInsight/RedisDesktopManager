#ifndef MODEL_H
#define MODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QVariant>
#include <QSharedPointer>
#include "items/treeitem.h"

namespace ConnectionsTree {

    class Model : public QAbstractItemModel
    {
        Q_OBJECT
    public:
        explicit Model(QObject *parent = 0);

        QVariant data(const QModelIndex &index, int role) const;
        QModelIndex index(int row, int column, const QModelIndex & parent) const;
        QModelIndex parent(const QModelIndex & index) const;
        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        inline int columnCount(const QModelIndex & parent = QModelIndex()) const { return 1; }

        QSharedPointer<TreeItem> getItemFromIndex(const QModelIndex&) const;

    protected:
        QList<QSharedPointer<TreeItem>> m_treeItems;

    };

}

#endif // MODEL_H
