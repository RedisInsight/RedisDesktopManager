#include "namespaceitem.h"
#include "connections-tree/iconproxy.h"
#include <QMenu>

#include "connections-tree/utils.h"

using namespace ConnectionsTree;

NamespaceItem::NamespaceItem(const QString &fullPath, unsigned short dbIndex,
                             QSharedPointer<Operations> operations,
                             QWeakPointer<TreeItem> parent)
    : m_fullPath(fullPath),
      m_dbIndex(dbIndex),
      m_operations(operations),
      m_parent(parent),      
      m_removed(false)
{
    m_displayName = m_fullPath.mid(m_fullPath.lastIndexOf(m_operations->getNamespaceSeparator())+1);
}

QString NamespaceItem::getDisplayName() const
{    
    return QString("%1 (%2)").arg(m_displayName).arg(childCount(true));
}

QString NamespaceItem::getName() const
{
    return m_displayName;
}

QIcon NamespaceItem::getIcon() const
{    
    return IconProxy::instance()->get(":/images/namespace.png");
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

bool NamespaceItem::onClick(TreeItem::ParentView&)
{
    return true;
}

QSharedPointer<QMenu> NamespaceItem::getContextMenu(TreeItem::ParentView&)
{
    QSharedPointer<QMenu> menu(new QMenu());

    if (!isEnabled())
        return menu;

    if (!m_signalReciever) {
        m_signalReciever = QSharedPointer<QObject>(new QObject());
    }

    menu->addAction(createMenuAction(":/images/delete.png", "Delete namespace",
                                     menu.data(), m_signalReciever.data(),
                                     [this](){ m_operations->deleteDbNamespace(*this); }));

    return menu;
}

bool NamespaceItem::isLocked() const
{
    return false;
}

bool NamespaceItem::isEnabled() const
{
    return m_removed == false;
}

void NamespaceItem::append(QSharedPointer<TreeItem> item)
{
    if (typeid(NamespaceItem)==typeid(*item)) {
        m_childNamespaces[item.staticCast<NamespaceItem>()->getName()] = qSharedPointerCast<NamespaceItem>(item);
    }
    m_childItems.append(item);
}

QByteArray NamespaceItem::getFullPath() const
{
    return m_fullPath.toUtf8();
}

int NamespaceItem::getDbIndex() const
{
    return m_dbIndex;
}

void NamespaceItem::setRemoved()
{
    m_removed = true;
}

QSharedPointer<NamespaceItem> NamespaceItem::findChildNamespace(const QString &name)
{
    if (!m_childNamespaces.contains(name))
        return QSharedPointer<NamespaceItem>();

    return m_childNamespaces[name];
}

