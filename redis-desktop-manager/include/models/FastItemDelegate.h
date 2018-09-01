#pragma once

#include <QStyledItemDelegate>
#include <QPainter>
#include <QModelIndex>
#include <QStyleOptionViewItem>

class FastItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const;
};

