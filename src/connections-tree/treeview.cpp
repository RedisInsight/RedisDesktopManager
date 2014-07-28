#include "treeview.h"
#include <QMouseEvent>
#include <QHeaderView>
#include <QMenu>

#include "items/treeitem.h"
#include "model.h"

using namespace ConnectionsTree;

TreeView::TreeView(QWidget * parent)
    : QTreeView(parent)
{
    header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    header()->setStretchLastSection(false);
    setUniformRowHeights(true);
    setContextMenuPolicy(Qt::CustomContextMenu);


    connect(this, &TreeView::clicked, this, &TreeView::processClick);
    connect(this, &TreeView::wheelClicked, this, &TreeView::processWheelClick);
    connect(this, &TreeView::customContextMenuRequested, this, &TreeView::processContextMenu);
}

void TreeView::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::MiddleButton) {            
        emit wheelClicked(indexAt(event->pos()));
    }

    return QTreeView::mousePressEvent(event);
}

void TreeView::processContextMenu(const QPoint& point)
{
    if (point.isNull() || QCursor::pos().isNull())
        return;

    QSharedPointer<TreeItem> item = preProcessEvent(indexAt(point));

    if (item.isNull())
        return;

    item->getContextMenu(
                *static_cast<TreeItem::ParentView* const>(this),
                QWeakPointer<QTabWidget>()
                )->exec(point);
}

void TreeView::processClick(const QModelIndex& index)
{
    QSharedPointer<TreeItem> item = preProcessEvent(index);

    if (item.isNull())
        return;

    if (item->onClick(*static_cast<TreeItem::ParentView*>(this),
                      QWeakPointer<QTabWidget>())) {
        setExpanded(index, true);
    }
}

void TreeView::processWheelClick(const QModelIndex& index)
{
    QSharedPointer<TreeItem> item = preProcessEvent(index);

    if (item.isNull())
        return;

    item->onWheelClick(*static_cast<TreeItem::ParentView*>(this),
                       QWeakPointer<QTabWidget>());
}


void TreeView::setModel(Model *model)
{
    QTreeView::setModel(static_cast<QAbstractItemModel*>(model));
}

const Model *TreeView::model() const
{
    return qobject_cast<Model*>(QTreeView::model());
}

QWidget *TreeView::getParentWidget()
{
    return dynamic_cast<QWidget*>(this);
}

QSharedPointer<TreeItem> TreeView::preProcessEvent(const QModelIndex &index)
{
    if (!index.isValid())
        return QSharedPointer<TreeItem>();

    QSharedPointer<TreeItem> item = model()->getItemFromIndex(index);

    if (item->isLocked())
        return QSharedPointer<TreeItem>();

    return item;
}
