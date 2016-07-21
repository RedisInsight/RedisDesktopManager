#include "treeitem.h"
#include "connections-tree/model.h"

ConnectionsTree::TreeItem::TreeItem(Model &m)
    : m_model(m)
{

}

int ConnectionsTree::TreeItem::row() const
{
    if (!parent())
        return 0;

    auto p = parent().toStrongRef();

    for (uint index = 0; index < p->childCount(); ++index)
    {
        if (p->child(index).data() == this)
            return index;
    }

    return 0;
}

QWeakPointer<ConnectionsTree::TreeItem> ConnectionsTree::TreeItem::getSelf()
{
    if (m_selfPtr)
        return m_selfPtr;

    if (!parent())
        return QWeakPointer<TreeItem>();

    QSharedPointer<TreeItem> p = parent().toStrongRef();

    if (!p)
        return QWeakPointer<TreeItem>();

    m_selfPtr = p->child(row()).toWeakRef();

    return m_selfPtr;
}

void ConnectionsTree::TreeItem::onWheelClick(ConnectionsTree::TreeItem::ParentView &treeView)
{
    Q_UNUSED(treeView);
}

ConnectionsTree::Model &ConnectionsTree::TreeItem::model()
{
    return m_model;
}
