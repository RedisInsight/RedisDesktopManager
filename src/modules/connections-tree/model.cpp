#include "model.h"
#include "items/serveritem.h"
#include <QWeakPointer>
#include <QDebug>
using namespace ConnectionsTree;

Model::Model(QObject *parent) :
    QAbstractItemModel(parent),
    m_rawPointers(new QHash<TreeItem*, QWeakPointer<TreeItem>>())
{    
    QObject::connect(this, &Model::itemChanged, this, &Model::onItemChanged);
    QObject::connect(this, &Model::itemChildsLoaded, this, &Model::onItemChildsLoaded);
    QObject::connect(this, &Model::itemChildsUnloaded, this, &Model::onItemChildsUnloaded);

    qRegisterMetaType<QWeakPointer<TreeItem>>("QWeakPointer<TreeItem>");
}

QVariant Model::data(const QModelIndex &index, int role) const
{
    const TreeItem *item = getItemFromIndex(index);

    if (item == nullptr)
        return QVariant();

    switch (role) {
        case Qt::DisplayRole: return item->getDisplayName();
        case Qt::DecorationRole: return item->getIcon();        
        case Qt::SizeHintRole: return QSize(100 + item->getDisplayName().size() * 5, 18 );
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
    if (parentItem) {
        childItem = parentItem->child(row);
    } else if (row < m_treeItems.size()) {
        childItem = m_treeItems.at(row);
    }

    if (childItem.isNull())
        return QModelIndex();
    else {
        m_rawPointers->insert(childItem.data(), childItem.toWeakRef());
        return createIndex(row, column, childItem.data());
    }
}

QModelIndex Model::parent(const QModelIndex &index) const
{
    const TreeItem *childItem = getItemFromIndex(index);
    
    if (!childItem)
        return QModelIndex();

    QWeakPointer<TreeItem> parentItem = childItem->parent();

    if (!parentItem)
        return QModelIndex();

    m_rawPointers->insert(parentItem.data(), parentItem);
    return createIndex(parentItem.toStrongRef()->row(), 0, (void*)parentItem.data());
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

QModelIndex Model::getIndexFromItem(QWeakPointer<TreeItem> item)
{
    if (item && item.toStrongRef()) {
        return createIndex(item.toStrongRef()->row(), 0, (void*)item.data());
    }
    return QModelIndex();
}

bool Model::canFetchMore(const QModelIndex &parent) const
{
    TreeItem* i = getItemFromIndex(parent);

    return i && i->canFetchMore();
}

void Model::fetchMore(const QModelIndex &parent)
{
    TreeItem* i = getItemFromIndex(parent);

    if (!i)
        return;

    i->fetchMore();
}

void Model::onItemChanged(QWeakPointer<TreeItem> item)
{    
    if (!item)
        return;

    auto index = getIndexFromItem(item);

    if (!index.isValid())
        return;

    emit dataChanged(index, index);
}

void Model::onItemChildsLoaded(QWeakPointer<TreeItem> item)
{
    if (!item)
        return;

    auto index = getIndexFromItem(item);

    if (!index.isValid() || item.toStrongRef()->childCount() == 0)
        return;

    emit beginInsertRows(index, 0, item.toStrongRef()->childCount() - 1);
    emit endInsertRows();
}

void Model::onItemChildsUnloaded(QWeakPointer<TreeItem> item)
{
    if (!item)
        return;

    auto index = getIndexFromItem(item);

    if (!index.isValid())
        return;

    emit beginRemoveRows(index, 0, item.toStrongRef()->childCount() - 1);
    emit endRemoveRows();
}

void Model::addRootItem(QSharedPointer<ServerItem> item)
{
    if (item.isNull())
        return;

    int insertIndex = m_treeItems.size();

    emit beginInsertRows(QModelIndex(), insertIndex, insertIndex);

    item->setRow(insertIndex);
    item->setWeakPointer(item.toWeakRef());
    m_treeItems.push_back(item);   

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
