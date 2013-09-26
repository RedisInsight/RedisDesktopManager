#include "hashViewTab.h"
#include "HashKeyModel.h"

hashViewTab::hashViewTab(QString keyName, QStringList keyValue)	
{
	ui.setupUi(this);

	ui.keyName->setText(keyName);

	HashKeyModel * model = new HashKeyModel(keyValue);

	ui.keyValue->setModel(model);
	ui.keyValue->setVisible(false);
	ui.keyValue->resizeRowsToContents();
	ui.keyValue->setVisible(true);
}

