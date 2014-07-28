#include "model.h"
#include "items/serveritem.h"
#include <QDebug>
using namespace ConnectionsTree;

Model::Model(QObject *parent) :
    QAbstractItemModel(parent)
{
}

QVariant Model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const TreeItem *item = static_cast<const TreeItem*>(index.internalPointer());

    if (item == nullptr)
        return QVariant();

    switch (role) {
        case Qt::DisplayRole: return item->getDisplayName();
        case Qt::DecorationRole: return item->getIcon();
    }

    return QVariant();
}

QModelIndex Model::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    const TreeItem* parentItem = (parent.isValid())?
             static_cast<const TreeItem*>(parent.internalPointer()) : nullptr;


    QSharedPointer<TreeItem> childItem;

    if (parentItem == nullptr) {

       if (row >= m_treeItems.size())
            return QModelIndex();

       childItem = m_treeItems.at(row);

    } else {
       childItem = parentItem->child(row);
    }

    if (childItem.isNull())
        return QModelIndex();
    else
        return createIndex(row, column, childItem.data());
}

QModelIndex Model::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    const TreeItem *childItem = static_cast<const TreeItem*>(index.internalPointer());
    const TreeItem* parentItem = childItem->parent();

    if (parentItem == nullptr)
        return QModelIndex();

    return createIndex(/*parentItem->row()*/3, 0, (void*)parentItem);
}

int Model::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return m_treeItems.size();

    if (parent.column() > 0)
        return 0;

    const TreeItem *parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

TreeItem* Model::getItemFromIndex(const QModelIndex &index) const
{
    if (index.internalPointer() != nullptr)
        return static_cast<TreeItem*>(index.internalPointer());

    return nullptr;
}

void Model::addRootItem(QSharedPointer<ServerItem> item)
{
    if (item.isNull())
        return;

    int insertIndex = m_treeItems.size();

    emit beginInsertRows(QModelIndex(), insertIndex, insertIndex);    

    m_treeItems.push_back(item);

    QModelIndex itemIndex = index(insertIndex, 0, QModelIndex());

    connect(item.data(), &ServerItem::databaseListLoaded,
            this, [this, itemIndex, item]()
    {
        emit beginInsertRows(itemIndex, 0, item->childCount()-1);
        qDebug() << "database list loaded";
        emit endInsertRows();
    });

    connect(item.data(), &ServerItem::keysLoadedInDatabase,
            this, [this, itemIndex, item](unsigned int dbIndex)
    {
        emit beginInsertRows(itemIndex, 0, item->child(dbIndex)->childCount());
        qDebug() << "key list loaded";
        emit endInsertRows();
    });

    emit endInsertRows();
}
