#ifndef ZSETVIEWTAB_H
#define ZSETVIEWTAB_H

#include <QWidget>
#include "ui_zsetViewTab.h"

class zsetViewTab : public QWidget
{
	Q_OBJECT

public:
	zsetViewTab(QString keyName, QStringList keyValue);

private:
	Ui::zsetViewTab ui;
};

#endif // ZSETVIEWTAB_H
