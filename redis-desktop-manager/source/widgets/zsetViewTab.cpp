#include "zsetViewTab.h"
#include "SortedSetKeyModel.h"

zsetViewTab::zsetViewTab(QString keyName, QStringList keyValue)
	: QWidget(nullptr)
{
	ui.setupUi(this);

	ui.keyName->setText(keyName);

	SortedSetKeyModel * model = new SortedSetKeyModel(keyValue);

	ui.keyValue->setModel(model);
	ui.keyValue->setVisible(false);
	ui.keyValue->resizeRowsToContents();
	ui.keyValue->setVisible(true);
}
