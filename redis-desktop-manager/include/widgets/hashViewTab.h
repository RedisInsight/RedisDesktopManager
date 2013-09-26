#ifndef HASHVIEWTAB_H
#define HASHVIEWTAB_H

#include <QWidget>
#include "ui_hashViewTab.h"

class hashViewTab : public QWidget
{
	Q_OBJECT

public:
	hashViewTab(QString keyName, QStringList keyValue);

private:
	Ui::hashViewTab ui;
};

#endif // HASHVIEWTAB_H
