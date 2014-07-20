#ifndef DATABASEITEM_H
#define DATABASEITEM_H

#include "treeitem.h"
#include "connections-tree/operations.h"

namespace ConnectionsTree {

class DatabaseItem : public TreeItem
{
public:
    DatabaseItem(const QString& displayName, unsigned int index, int keysCount, QSharedPointer<Operations> operations);


    QString getDisplayName() const {return QString();}
    QIcon getIcon() const {return QIcon();}
    QList<QSharedPointer<TreeItem>> getAllChilds() const { return QList<QSharedPointer<TreeItem>>();}
    uint childCount() const {return 0;}
    QSharedPointer<TreeItem> child(int row) const {return QSharedPointer<TreeItem>();}
    QSharedPointer<TreeItem> parent() const {return QSharedPointer<TreeItem>();}

    bool onClick(QWeakPointer<ParentView> treeView, QWeakPointer<QTabWidget> tabs);
    void onWheelClick(QWeakPointer<ParentView> treeView, QWeakPointer<QTabWidget> tabs) {}
    QSharedPointer<QMenu> getContextMenu(QWeakPointer<ParentView> treeView, QWeakPointer<QTabWidget> tabs) {
        return QSharedPointer<QMenu>();
    }

    bool isLocked() const {return false;}

private:
};

}

#endif // DATABASEITEM_H
