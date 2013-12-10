#include "ConnectionTreeView.h"

#include <QMouseEvent>
#include <QHeaderView>
#include <QStandardItem>

ConnectionTreeView::ConnectionTreeView(QWidget * parent)
	: QTreeView(parent)
{
	header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	header()->setStretchLastSection(false);
	setUniformRowHeights(true);
	setContextMenuPolicy(Qt::CustomContextMenu);
}

void ConnectionTreeView::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::MiddleButton) {			
		emit wheelClicked(indexAt(event->pos()));
	}

	return QTreeView::mousePressEvent(event);
}

QStandardItem * ConnectionTreeView::getSelectedItem(int type)
{
	if (!selectionModel()->hasSelection())
		return nullptr;

	QModelIndexList selected = selectionModel()->selectedIndexes();

	if (selected.size() < 1) 
		return nullptr;

	QModelIndex index = selected.at(0);

	if (!index.isValid()) 
		return nullptr;

	QStandardItem * item = ((QStandardItemModel *)model())->itemFromIndex(index);	

	if (type != -1 && item->type() != type)
		return nullptr;	

	return item;		
}

