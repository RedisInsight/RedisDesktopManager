#include "ConnectionTreeView.h"

#include <QMouseEvent>

ConnectionTreeView::ConnectionTreeView(QWidget * parent)
	: QTreeView(parent)
{


}

void ConnectionTreeView::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::MiddleButton) {			
		emit wheelClicked(indexAt(event->pos()));
	}

	return QTreeView::mousePressEvent(event);
}