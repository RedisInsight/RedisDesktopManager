#include "loadmoreitem.h"

ConnectionsTree::LoadMoreItem::LoadMoreItem(QWeakPointer<TreeItem> parent, Model &model)
    : ConnectionsTree::TreeItem::TreeItem(model),
      m_parent(parent)
{

}

QString ConnectionsTree::LoadMoreItem::getDisplayName() const
{
    return QCoreApplication::translate("RESP", "Load more keys");
}

QList<QSharedPointer<ConnectionsTree::TreeItem> > ConnectionsTree::LoadMoreItem::getAllChilds() const
{
    return {};
}

bool ConnectionsTree::LoadMoreItem::supportChildItems() const
{
    return false;
}

uint ConnectionsTree::LoadMoreItem::childCount(bool) const
{
    return 0u;
}

QSharedPointer<ConnectionsTree::TreeItem> ConnectionsTree::LoadMoreItem::child(uint)
{
    return QSharedPointer<ConnectionsTree::TreeItem>();
}

QWeakPointer<ConnectionsTree::TreeItem> ConnectionsTree::LoadMoreItem::parent() const
{
    return m_parent;
}

bool ConnectionsTree::LoadMoreItem::isEnabled() const
{
    return true;
}

QHash<QString, std::function<bool()> > ConnectionsTree::LoadMoreItem::eventHandlers()
{
    QHash<QString, std::function<bool()>> events;
    events["click"] = [this]() {
        auto parentPtr = m_parent.toStrongRef();
        if (!parentPtr) {
            return true;
        }        
        parentPtr->fetchMore();
        return false;
    };
    return events;
}
