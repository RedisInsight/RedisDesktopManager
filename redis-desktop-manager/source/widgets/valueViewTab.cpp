#include "valueViewTab.h"
#include "HashKeyModel.h"
#include "RedisKeyItem.h"
#include "ListKeyModel.h"
#include "SortedSetKeyModel.h"
#include "ValueTabView.h"
#include "Command.h"
#include "ConnectionBridge.h"

ValueTab::ValueTab(RedisKeyItem * key)	
	: key(key), ui(nullptr)
{	
	ui = new ValueTabView(key->text(), this);	

	Command typeCmd = key->getTypeCommand();
	typeCmd.setOwner(this);
	typeCmd.setCallBackName("keyTypeLoaded");

	key->getConnection()->addCommand(typeCmd);

	/** Connect View SIGNALS to Controller SLOTS **/
	connect(ui->renameKey, SIGNAL(clicked()), this, SLOT(renameKey()));
}

void ValueTab::keyTypeLoaded(const QVariant & type)
{
	QString t = type.toString();
	ui->keyTypeLabelValue->setText(
		ui->keyTypeLabelValue->text()  + t.toUpper()
		);

	keyModel = key->getKeyModel(t);

	if (keyModel == nullptr) {
		emit error("Can not load key value");
		return;
	}

	connect(keyModel, SIGNAL(valueLoaded()), this, SLOT(valueLoaded()));

	keyModel->loadValue();
}

void ValueTab::valueLoaded()
{
	ui->initKeyValue(keyModel);

	setObjectName("valueTabReady");
}

void ValueTab::renameKey()
{
	keyModel->renameKey(ui->keyName->text());
}

void ValueTab::deleteKey()
{
	//todo implement this
}


