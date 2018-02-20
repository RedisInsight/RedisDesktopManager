#include "treeitem.h"
#include "connections-tree/model.h"

ConnectionsTree::TreeItem::TreeItem(Model &m)
    : m_model(m), m_locked(false)
{

}

QVariant ConnectionsTree::TreeItem::metadata(const QString &key) const
{
    if (!metadata().contains(key))
        return QVariant();

    return metadata()[key];
}

QVariantMap ConnectionsTree::TreeItem::metadata() const
{
    QVariantMap meta;
    meta["name"] = getDisplayName();
    meta["full_name"] = getName();
    meta["type"] = getType();
    meta["locked"] = isLocked();
    meta["state"] = isEnabled();    
    return meta;
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

ConnectionsTree::Model &ConnectionsTree::TreeItem::model()
{
    return m_model;
}

void ConnectionsTree::TreeItem::lock()
{
    m_locked = true;
    emit m_model.itemChanged(getSelf());
}

void ConnectionsTree::TreeItem::unlock()
{
    m_locked = false;
    emit m_model.itemChanged(getSelf());
}

void ConnectionsTree::TreeItem::handleEvent(QString event)
{
    if (!m_eventHandlers.contains(event))
        return;

    if (isLocked()) {
        qDebug() << "Item is locked. Ignore event: " << event;
        return;
    }

    try {
        m_eventHandlers[event]();
    } catch (...) {
        qWarning() << "Error on event processing: " << event;
    }
}
