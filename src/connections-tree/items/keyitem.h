#ifndef KEYITEM_H
#define KEYITEM_H

#include "treeitem.h"

namespace ConnectionsTree {

    class KeyItem : public TreeItem
    {
    public:
        KeyItem();

        bool onClick(QWeakPointer<QWidget> treeView, QWeakPointer<QTabWidget> tabs);
        void onWheelClick(QWeakPointer<QWidget> treeView, QWeakPointer<QTabWidget> tabs);
        QSharedPointer<QMenu> getContextMenu(QWeakPointer<QWidget> treeView, QWeakPointer<QTabWidget> tabs);
    };

}
#endif // KEYITEM_H
