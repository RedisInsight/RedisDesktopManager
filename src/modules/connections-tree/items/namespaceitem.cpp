#include "namespaceitem.h"
#include <QMenu>

using namespace ConnectionsTree;

NamespaceItem::NamespaceItem(const QByteArray &fullPath,
                             QSharedPointer<Operations> operations,
                             QWeakPointer<TreeItem> parent)
    : m_fullPath(fullPath),
      m_operations(operations),
      m_parent(parent),
      m_locked(false)
{
    m_displayName = m_fullPath.mid(m_fullPath.lastIndexOf(m_operations->getNamespaceSeparator())+1);
}

QString NamespaceItem::getDisplayName() const
{    
    return QString("%1 (%2)").arg(m_displayName).arg(childCount(true));
}

QString NamespaceItem::getDisplayPart() const
{
    return m_displayName;
}

QByteArray NamespaceItem::getName() const
{
    return m_fullPath;
}

QString NamespaceItem::getIconUrl() const
{    
    return QString("qrc:/images/namespace.png");
}

QList<QSharedPointer<TreeItem> > NamespaceItem::getAllChilds() const
{
    return m_childItems;
}

uint NamespaceItem::childCount(bool recursive) const
{
    if (!recursive)
        return m_childItems.size();

    uint count = 0;
    for (auto item : m_childItems) {
        if (item->supportChildItems()) {
            count += item->childCount(true);
        } else {
            count += 1;
        }
    }
    return count;
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
        m_childNamespaces[item.staticCast<NamespaceItem>()->getDisplayPart()] = qSharedPointerCast<NamespaceItem>(item);
    }
    m_childItems.append(item);
}

QSharedPointer<NamespaceItem> NamespaceItem::findChildNamespace(const QString &name)
{
    if (!m_childNamespaces.contains(name))
        return QSharedPointer<NamespaceItem>();

    return m_childNamespaces[name];
}
