#include "listViewTab.h"
#include "ListKeyModel.h"

listViewTab::listViewTab(QString keyName, QStringList keyValue)
	: QWidget(nullptr)
{
	ui.setupUi(this);

	ui.keyName->setText(keyName);

	ListKeyModel * model = new ListKeyModel(keyValue);

	ui.keyValue->setModel(model);
	ui.keyValue->setVisible(false);
	ui.keyValue->resizeRowsToContents();
	ui.keyValue->setVisible(true);
}

