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

QSharedPointer<TreeItem> KeyItem::child(int row) const
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
        m_operations->openKeyTab(m_fullPath, m_dbIndex, false);

    return false;
}

void KeyItem::onWheelClick(ParentView&)
{    
    if (isEnabled())
        m_operations->openKeyTab(m_fullPath, m_dbIndex, true);
}

QSharedPointer<QMenu> KeyItem::getContextMenu(ParentView&)
{
    QSharedPointer<QMenu> menu(new QMenu());

    QAction* openKey = new QAction(QIcon(":/images/add.png"), "Open key", menu.data());
    QObject::connect(openKey, &QAction::triggered, [this] { m_operations->openKeyTab(m_fullPath, m_dbIndex, false); });
    menu->addAction(openKey);

    QAction* openInNewTab = new QAction(QIcon(":/images/add.png"), "Open key value in new tab", menu.data());
    QObject::connect(openInNewTab, &QAction::triggered, [this] { m_operations->openKeyTab(m_fullPath, m_dbIndex, true); });
    menu->addAction(openInNewTab);

//    menu->addSeparator();

//    QAction* rename = new QAction(QIcon(":/images/editdb.png"), "Rename key", menu.data());
//    menu->addAction(rename);

//    QAction* remove = new QAction(QIcon(":/images/delete.png"), "Delete key", menu.data());
//    menu->addAction(remove);

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

