#include "hashViewTab.h"
#include "HashKeyModel.h"
#include "RedisKeyItem.h"

hashViewTab::hashViewTab(RedisKeyItem * key)	
{
	ui.setupUi(this);

	ui.keyName->setText(key->text());

	HashKeyModel * model = new HashKeyModel(key->getValue().toStringList());

	ui.keyValue->setModel(model);
	ui.keyValue->setVisible(false);
	ui.keyValue->resizeRowsToContents();
	ui.keyValue->setVisible(true);
}

