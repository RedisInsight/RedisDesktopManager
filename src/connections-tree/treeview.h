#pragma once
#include <QTreeView>
#include <QSharedPointer>

namespace ConnectionsTree {

    class Model;
    class TreeItem;

    class TreeView : public QTreeView
    {
        Q_OBJECT

    public:
        TreeView(QWidget *);

        void setModel(Model *model);
        const Model* model() const;

    protected:
        void mousePressEvent(QMouseEvent * event);

    protected slots:
        void processClick(const QModelIndex&);
        void processWheelClick(const QModelIndex&);
        void processContextMenu(const QPoint &);

    private:
        QSharedPointer<TreeItem> preProcessEvent(const QModelIndex& index);

    signals:
        void wheelClicked(const QModelIndex & index);

    };
}
