#pragma once
#include <functional>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QWeakPointer>
#include <QString>
#include <QIcon>
#include <QList>
#include <QWidget>
#include <QMenu>
#include <QModelIndex>

namespace ConnectionsTree {

class Model;

class TreeItem {
public:

    class ParentView {

    public:        
        virtual QWidget* getParentWidget() = 0;

    };

    TreeItem(Model& m);

    virtual ~TreeItem() {}

    virtual QString getDisplayName() const = 0;

    virtual QIcon getIcon() const = 0;

    virtual QList<QSharedPointer<TreeItem>> getAllChilds() const = 0;

    virtual uint childCount(bool recursive = false) const = 0;

    virtual QSharedPointer<TreeItem> child(uint row) const = 0;

    virtual QWeakPointer<TreeItem> parent() const = 0;

    virtual bool supportChildItems() const { return true; }    

    virtual int row() const;

    virtual QWeakPointer<TreeItem> getSelf();

    virtual bool onClick(ParentView& treeView) = 0;

    virtual void onWheelClick(ParentView& treeView);

    virtual QSharedPointer<QMenu> getContextMenu(ParentView& treeView) = 0;

    virtual bool isLocked() const = 0;

    virtual bool isEnabled() const = 0;

    virtual bool canFetchMore() const { return false; }

    virtual void fetchMore() {}

    virtual Model& model();

protected:
     Model& m_model;

private:
     QWeakPointer<TreeItem> m_selfPtr;
};

typedef QList<QSharedPointer<TreeItem>> TreeItems;

}
