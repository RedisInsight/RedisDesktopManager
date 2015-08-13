#include "namespaceitem.h"
#include "connections-tree/iconproxy.h"
#include <QMenu>

using namespace ConnectionsTree;

NamespaceItem::NamespaceItem(const QString &fullPath,
                             QSharedPointer<Operations> operations,
                             QWeakPointer<TreeItem> parent)
    : m_fullPath(fullPath),
      m_operations(operations),
      m_parent(parent),
      m_locked(false)
{

}

QString NamespaceItem::getDisplayName() const
{
    int lastIndex = m_fullPath.lastIndexOf(m_operations->getNamespaceSeparator());

    return m_fullPath.mid(lastIndex+1);
}

QIcon NamespaceItem::getIcon() const
{    
    return IconProxy::instance()->get(":/images/namespace.png");
}

QList<QSharedPointer<TreeItem> > NamespaceItem::getAllChilds() const
{
    return m_childItems;
}

uint NamespaceItem::childCount() const
{
    return m_childItems.size();
}

QSharedPointer<TreeItem> NamespaceItem::child(uint row) const
{    
    if (row < m_childItems.size())
        return m_childItems.at(row);

    return QSharedPointer<TreeItem>();
}

QWeakPointer<TreeItem> NamespaceItem::parent() const
{
    return m_parent;
}

bool NamespaceItem::onClick(TreeItem::ParentView&)
{
    return true;
}

QSharedPointer<QMenu> NamespaceItem::getContextMenu(TreeItem::ParentView&)
{
    return QSharedPointer<QMenu>();
}

bool NamespaceItem::isLocked() const
{
    return m_locked;
}

bool NamespaceItem::isEnabled() const
{
    return true;
}

void NamespaceItem::append(QSharedPointer<TreeItem> item)
{
    if (typeid(NamespaceItem)==typeid(*item)) {
        m_childNamespaces[item->getDisplayName()] = qSharedPointerCast<NamespaceItem>(item);
    }
    m_childItems.append(item);
}

QSharedPointer<NamespaceItem> NamespaceItem::findChildNamespace(const QString &name)
{
    if (!m_childNamespaces.contains(name))
        return QSharedPointer<NamespaceItem>();

    return m_childNamespaces[name];
}

