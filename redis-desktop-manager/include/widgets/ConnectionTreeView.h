#pragma once
#include <QTreeView>

class ConnectionTreeView : public QTreeView
{
	Q_OBJECT

public:
	ConnectionTreeView(QWidget *);	

protected:
	void mousePressEvent(QMouseEvent * event);

signals:
	void wheelClicked(const QModelIndex & index);

};

