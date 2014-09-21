#pragma once
#include <QTreeView>
#include <QSharedPointer>

#include "connections-tree/items/treeitem.h"

namespace ConnectionsTree {

    class Model;    
    class TabWidget;

    class TreeView : public QTreeView, public TreeItem::ParentView
    {
        Q_OBJECT

    public:
        TreeView(QWidget *);

        void setModel(Model *model);
        const Model* model() const;

        QWidget* getParentWidget() override;

    protected:
        void mousePressEvent(QMouseEvent * event);

    protected slots:
        void processClick(const QModelIndex&);
        void processWheelClick(const QModelIndex&);
        void processContextMenu(const QPoint &);

    private:
        TreeItem *preProcessEvent(const QModelIndex& index);

    signals:
        void wheelClicked(const QModelIndex & index);

    };
}
