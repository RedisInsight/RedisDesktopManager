#ifndef LISTVIEWTAB_H
#define LISTVIEWTAB_H

#include <QWidget>
#include "ui_listViewTab.h"

class listViewTab : public QWidget
{
	Q_OBJECT

public:
	listViewTab(QString keyName, QStringList keyValue);

private:
	Ui::listViewTab ui;
};

#endif // LISTVIEWTAB_H
