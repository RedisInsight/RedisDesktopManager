#include "keyitem.h"
#include "connections-tree/iconproxy.h"
#include <QMenu>

using namespace ConnectionsTree;

KeyItem::KeyItem(const QString& fullPath, int dbIndex, QSharedPointer<Operations> operations, const TreeItem* parent)
    : m_fullPath(fullPath),
      m_dbIndex(dbIndex),
      m_operations(operations),
      m_parent(parent),
      m_locked(false),
      m_removed(false)
{
}

QString KeyItem::getDisplayName() const
{
    return m_fullPath;
}

QIcon KeyItem::getIcon() const
{
    return IconProxy::instance()->get(":/images/key.png");
}

QList<QSharedPointer<TreeItem>> KeyItem::getAllChilds() const
{
    return QList<QSharedPointer<TreeItem>>();
}

uint KeyItem::childCount() const
{
    return (uint)0;
}

QSharedPointer<TreeItem> KeyItem::child(uint row) const
{
    Q_UNUSED(row);

    return QSharedPointer<TreeItem>();
}

const TreeItem *KeyItem::parent() const
{
    return m_parent;
}

bool KeyItem::onClick(ParentView&)
{
    if (isEnabled())
        m_operations->openKeyTab(*this, false);

    return false;
}

void KeyItem::onWheelClick(ParentView&)
{    
    if (isEnabled())
        m_operations->openKeyTab(*this, true);
}

QSharedPointer<QMenu> KeyItem::getContextMenu(ParentView&)
{
    QSharedPointer<QMenu> menu(new QMenu());

    QAction* openKey = new QAction(QIcon(":/images/add.png"), "Open key", menu.data());
    QObject::connect(openKey, &QAction::triggered, [this] { m_operations->openKeyTab(*this, false); });
    menu->addAction(openKey);

    QAction* openInNewTab = new QAction(QIcon(":/images/add.png"), "Open key value in new tab", menu.data());
    QObject::connect(openInNewTab, &QAction::triggered, [this] { m_operations->openKeyTab(*this, true); });
    menu->addAction(openInNewTab);

    return menu;
}

bool KeyItem::isLocked() const
{
    return m_locked;
}

bool KeyItem::isEnabled() const
{
    return isLocked() == false && m_removed == false;
}

QString KeyItem::getFullPath() const
{
    return m_fullPath;
}

int KeyItem::getDbIndex() const
{
    return m_dbIndex;
}

void KeyItem::setRemoved()
{
    m_removed = true;
}

