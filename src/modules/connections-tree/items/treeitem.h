#ifndef TREEITEM_H
#define TREEITEM_H

#define QT_SHAREDPOINTER_TRACK_POINTERS
#include <QSharedPointer>
#include <QWeakPointer>
#include <QString>
#include <QIcon>
#include <QList>
#include <QWidget>

class QMenu;

namespace ConnectionsTree {

class TreeItem {
public:

    class ParentView {

    public:        
        virtual QWidget* getParentWidget() = 0;

    };

    TreeItem() {}

    virtual QString getDisplayName() const = 0;
    virtual QIcon getIcon() const = 0;
    virtual QList<QSharedPointer<TreeItem>> getAllChilds() const = 0;
    virtual uint childCount() const = 0;
    virtual QSharedPointer<TreeItem> child(int row) const = 0;
    virtual const TreeItem* parent() const = 0;

    virtual int row() const
    {
        if (!parent())
            return 0;

        for (int index = 0; index < parent()->childCount(); ++index)
        {
            if (parent()->child(index).data() == this)
                return index;
        }

        return 0;
    }

    virtual bool onClick(ParentView& treeView) = 0;
    virtual void onWheelClick(ParentView& treeView) = 0;
    virtual QSharedPointer<QMenu> getContextMenu(ParentView& treeView) = 0;

    virtual bool isLocked() const = 0;

    virtual ~TreeItem() {}
};
}
#endif // TREEITEM_H
