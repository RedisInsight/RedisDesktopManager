#ifndef STRINGVIEWTAB_H
#define STRINGVIEWTAB_H

#include <QWidget>
#include "ui_stringViewTab.h"

class stringViewTab : public QWidget
{
	Q_OBJECT

public:
	stringViewTab(QString keyName, QString keyValue);	

private:
	Ui::stringViewTab ui;
};

#endif // STRINGVIEWTAB_H
