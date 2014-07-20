#ifndef TREEITEM_H
#define TREEITEM_H

#include <QSharedPointer>
#include <QWeakPointer>
#include <QString>
#include <QIcon>
#include <QList>
#include <QWidget>

class QMenu;
class QTabWidget;

namespace ConnectionsTree {

class TreeItem {
public:

    class ParentView {

    public:
        virtual void removeCurrentItem() = 0;
        virtual QWidget* getParentWidget() = 0;

    };

    virtual QString getDisplayName() const = 0;
    virtual QIcon getIcon() const = 0;
    virtual QList<QSharedPointer<TreeItem>> getAllChilds() const = 0;
    virtual uint childCount() const = 0;
    virtual QSharedPointer<TreeItem> child(int row) const = 0;
    virtual QSharedPointer<TreeItem> parent() const = 0;

    virtual bool onClick(QWeakPointer<ParentView> treeView, QWeakPointer<QTabWidget> tabs) = 0;
    virtual void onWheelClick(QWeakPointer<ParentView> treeView, QWeakPointer<QTabWidget> tabs) = 0;
    virtual QSharedPointer<QMenu> getContextMenu(QWeakPointer<ParentView> treeView, QWeakPointer<QTabWidget> tabs) = 0;

    virtual bool isLocked() const = 0;
};

}
#endif // TREEITEM_H
