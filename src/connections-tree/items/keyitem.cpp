#include "keyitem.h"
#include <QMenu>

using namespace ConnectionsTree;

KeyItem::KeyItem()
{
}

bool KeyItem::onClick(QWeakPointer<QWidget> treeView, QWeakPointer<QTabWidget> tabs)
{
//    if (item->isEnabled())
//        ui.tabWidget->openKeyTab(dynamic_cast<RedisKeyItem *>(item));

    return false;
}

void KeyItem::onWheelClick(QWeakPointer<QWidget> treeView, QWeakPointer<QTabWidget> tabs)
{
//     ui.tabWidget->openKeyTab((RedisKeyItem *)item, true);
}

QSharedPointer<QMenu> KeyItem::getContextMenu(QWeakPointer<QWidget> treeView, QWeakPointer<QTabWidget> tabs)
{
    QSharedPointer<QMenu> menu(new QMenu());
    //menu->addAction("Open key value in new tab", this, SLOT(OnKeyOpenInNewTab()));

    return menu;
}

//void MainWin::OnKeyOpenInNewTab()
//{
//    QStandardItem * item = ui.serversTreeView->getSelectedItem();

//    if (item == nullptr || item->type() != RedisKeyItem::TYPE)
//        return;

//    ui.tabWidget->openKeyTab((RedisKeyItem *)item, true);
//}
