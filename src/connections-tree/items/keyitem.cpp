#include "keyitem.h"
#include "connections-tree/iconproxy.h"
#include <QMenu>

using namespace ConnectionsTree;

KeyItem::KeyItem(const QString& fullPath, QSharedPointer<Operations> operations, const TreeItem* parent)
    : m_fullPath(fullPath),
      m_operations(operations),
      m_parent(parent),
      m_locked(false)
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

bool KeyItem::onClick(ParentView& treeView, QWeakPointer<QTabWidget> tabs)
{
    //    if (item->isEnabled())
//        ui.tabWidget->openKeyTab(dynamic_cast<RedisKeyItem *>(item));

    return false;
}

void KeyItem::onWheelClick(ParentView& treeView, QWeakPointer<QTabWidget> tabs)
{
//     ui.tabWidget->openKeyTab((RedisKeyItem *)item, true);
}

QSharedPointer<QMenu> KeyItem::getContextMenu(ParentView& treeView, QWeakPointer<QTabWidget> tabs)
{
    QSharedPointer<QMenu> menu(new QMenu());
    //menu->addAction("Open key value in new tab", this, SLOT(OnKeyOpenInNewTab()));

    return menu;
}

bool KeyItem::isLocked() const
{
    return m_locked;
}

//void MainWin::OnKeyOpenInNewTab()
//{
//    QStandardItem * item = ui.serversTreeView->getSelectedItem();

//    if (item == nullptr || item->type() != RedisKeyItem::TYPE)
//        return;

//    ui.tabWidget->openKeyTab((RedisKeyItem *)item, true);
//}
