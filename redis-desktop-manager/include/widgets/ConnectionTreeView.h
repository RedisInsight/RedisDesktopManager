#pragma once
#include <QTreeView>

class QStandardItem;

class ConnectionTreeView : public QTreeView
{
	Q_OBJECT

public:
	ConnectionTreeView(QWidget *);	

	QStandardItem * getSelectedItem(int type = -1);

protected:
	void mousePressEvent(QMouseEvent * event);

signals:
	void wheelClicked(const QModelIndex & index);

};

