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
    const TreeItem *item = getItemFromIndex(index);

    if (item == nullptr)
        return QVariant();

    switch (role) {
        case Qt::DisplayRole: return item->getDisplayName();
        case Qt::DecorationRole: return item->getIcon();
    }

    return QVariant();
}

Qt::ItemFlags Model::flags(const QModelIndex &index) const
{
    const TreeItem *item = getItemFromIndex(index);

    if (item == nullptr)    
        return Qt::NoItemFlags;

    Qt::ItemFlags result = Qt::ItemIsSelectable;

    if (item->isEnabled())
        result |= Qt::ItemIsEnabled;

    return result;
}

QModelIndex Model::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    const TreeItem* parentItem = getItemFromIndex(parent);
    QSharedPointer<TreeItem> childItem;

    // get item from root items
    if (!parentItem && row < m_treeItems.size()) {
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
    const TreeItem *childItem = getItemFromIndex(index);
    
    if (!childItem)
        return QModelIndex();

    const TreeItem *parentItem = childItem->parent();

    if (!parentItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, (void*)parentItem);
}

int Model::rowCount(const QModelIndex &parent) const
{
    const TreeItem* parentItem = getItemFromIndex(parent);
    
    if (!parentItem)
        return m_treeItems.size();

    if (parent.column() > 0)
        return 0;

    return parentItem->childCount();
}

void Model::addRootItem(QSharedPointer<ServerItem> item)
{
    if (item.isNull())
        return;

    int insertIndex = m_treeItems.size();
    emit beginInsertRows(QModelIndex(), insertIndex, insertIndex);
    item->setRow(insertIndex);
    m_treeItems.push_back(item);

    QModelIndex itemIndex = index(insertIndex, 0, QModelIndex());

    connect(item.data(), &ServerItem::databaseListLoaded,
            this, [this, itemIndex, item]()
    {
        emit beginInsertRows(itemIndex, 0, item->childCount()-1);
        emit endInsertRows();
    });

    connect(item.data(), &ServerItem::updateIcon,
            this, [this, itemIndex, item]()
    {
        emit dataChanged(itemIndex, itemIndex);
    });

    connect(item.data(), &ServerItem::unloadStarted,
            this, [this, itemIndex, item]()
    {
        emit beginRemoveRows(itemIndex, 0, item->childCount()-1);
        emit endRemoveRows();
    });

    connect(item.data(), &ServerItem::keysLoadedInDatabase,
            this, [this, itemIndex, item](unsigned int dbIndex)
    {
        QModelIndex dbModelIndex = index(dbIndex, 0, itemIndex);
        emit beginInsertRows(dbModelIndex, 0, item->child(dbIndex)->childCount() - 1);
        emit endInsertRows();
    });

    connect(item.data(), &ServerItem::updateDbIcon,
            this, [this, itemIndex, item](unsigned int dbIndex)
    {
        QModelIndex dbModelIndex = index(dbIndex, 0, itemIndex);
        emit dataChanged(dbModelIndex, dbModelIndex);
    });

    connect(item.data(), &ServerItem::unloadStartedInDatabase,
            this, [this, itemIndex, item](unsigned int dbIndex)
    {
        QModelIndex dbModelIndex = index(dbIndex, 0, itemIndex);
        emit beginRemoveRows(dbModelIndex, 0, item->child(dbIndex)->childCount() - 1);
        emit endRemoveRows();
    });

    emit endInsertRows();
}

void Model::removeRootItem(QSharedPointer<ServerItem> item)
{
    if (!item)
        return;

    beginRemoveRows(QModelIndex(), item->row(), item->row());
    m_treeItems.removeAll(item);
    endRemoveRows();
}
