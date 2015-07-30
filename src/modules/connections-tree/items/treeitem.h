#pragma once
#include <functional>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QString>
#include <QIcon>
#include <QList>
#include <QWidget>
#include <QMenu>

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
    virtual QSharedPointer<TreeItem> child(uint row) const = 0;
    virtual QWeakPointer<TreeItem> parent() const = 0;

    virtual int row() const
    {
        if (!parent())
            return 0;

        auto p = parent().toStrongRef();

        for (uint index = 0; index < p->childCount(); ++index)
        {
            if (p->child(index).data() == this)
                return index;
        }

        return 0;
    }

    virtual bool onClick(ParentView& treeView) = 0;

    virtual void onWheelClick(ParentView& treeView)
    {
        Q_UNUSED(treeView);
    }

    virtual QSharedPointer<QMenu> getContextMenu(ParentView& treeView) = 0;

    virtual bool isLocked() const = 0;
    virtual bool isEnabled() const = 0;

    virtual ~TreeItem() {}
};

}
