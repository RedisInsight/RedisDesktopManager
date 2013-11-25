#include "FastItemDelegate.h"

void FastItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
		   const QModelIndex &index) const
{
	painter->setPen(Qt::black);
	painter->drawText(option.rect, option.displayAlignment, index.data().toString());
}

