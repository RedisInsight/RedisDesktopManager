#include "treeview.h"
#include <QMouseEvent>
#include <QHeaderView>
#include <QMenu>

#include "items/treeitem.h"
#include "model.h"

using namespace ConnectionsTree;

TreeView::TreeView(QWidget * parent)
    : QTreeView(parent), m_tabWidget(nullptr)
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
    Q_ASSERT(m_tabWidget != nullptr);

    if (point.isNull() || QCursor::pos().isNull())
        return;

    TreeItem* item = preProcessEvent(indexAt(point));

    if (item == nullptr)
        return;


    item->getContextMenu(
                *static_cast<TreeItem::ParentView* const>(this),
                *m_tabWidget
                )->exec(mapToGlobal(point));
}

void TreeView::processClick(const QModelIndex& index)
{
    Q_ASSERT(m_tabWidget != nullptr);

    TreeItem* item = preProcessEvent(index);

    if (item == nullptr)
        return;


    if (item->onClick(*static_cast<TreeItem::ParentView*>(this),
                      *m_tabWidget)) {
        setExpanded(index, true);
    }
}

void TreeView::processWheelClick(const QModelIndex& index)
{
    Q_ASSERT(m_tabWidget != nullptr);

    TreeItem* item = preProcessEvent(index);

    if (item == nullptr)
        return;

    item->onWheelClick(*static_cast<TreeItem::ParentView*>(this),
                       *m_tabWidget);
}


void TreeView::setModel(Model *model)
{
    QTreeView::setModel(static_cast<QAbstractItemModel*>(model));
}

const Model *TreeView::model() const
{
    return qobject_cast<Model*>(QTreeView::model());
}

void TreeView::setTabWidget(TabWidget *widget)
{
    m_tabWidget = widget;
}

QWidget *TreeView::getParentWidget()
{
    return dynamic_cast<QWidget*>(this);
}

TreeItem* TreeView::preProcessEvent(const QModelIndex &index)
{
    if (!index.isValid())
        return nullptr;

    TreeItem* item = model()->getItemFromIndex(index);

    if (item == nullptr || item->isLocked())
        return nullptr;

    return item;
}
