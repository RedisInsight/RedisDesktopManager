#include "stringViewTab.h"

stringViewTab::stringViewTab(QString keyName, QString keyValue)
{
	ui.setupUi(this);

	ui.keyName->setText(keyName);
	ui.keyValue->setPlainText(keyValue);
}
