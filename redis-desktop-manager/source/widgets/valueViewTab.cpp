#include "valueViewTab.h"
#include "HashKeyModel.h"
#include "RedisKeyItem.h"
#include "ListKeyModel.h"
#include "SortedSetKeyModel.h"
#include "ValueTabView.h"
#include "Command.h"
#include "ConnectionBridge.h"
#include <QMessageBox>

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
	connect(ui->deleteKey, SIGNAL(clicked()), this, SLOT(deleteKey()));
	connect(ui, SIGNAL(saveChangedValue(const QString&, const QModelIndex *)),
		this, SLOT(updateValue(const QString&, const QModelIndex *)));

	connect(this, SIGNAL(error(const QString&)), this, SLOT(errorOccurred(const QString&)));
}

void ValueTab::keyTypeLoaded(Response type)
{
	QString t = type.getValue().toString();
	ui->keyTypeLabelValue->setText(
		ui->keyTypeLabelValue->text()  + t.toUpper()
		);

	keyModel = key->getKeyModel(t);

	if (keyModel == nullptr) {
		emit error("Can not load key value");
		return;
	}

	connect(keyModel, SIGNAL(valueLoaded()), this, SLOT(valueLoaded()));
	connect(keyModel, SIGNAL(keyRenameError(const QString&)), this, SIGNAL(error(const QString&)));
	connect(keyModel, SIGNAL(keyRenamed()), this, SLOT(keyRenamed()));
	connect(keyModel, SIGNAL(keyDeleteError(const QString&)), this, SIGNAL(error(const QString&)));
	connect(keyModel, SIGNAL(keyDeleted()), this, SLOT(keyDeleted()));
	connect(keyModel, SIGNAL(valueUpdateError(const QString&)), this, SIGNAL(error(const QString&)));
	connect(keyModel, SIGNAL(valueUpdated()), this, SLOT(valueUpdated()));

	keyModel->loadValue();
}

void ValueTab::valueLoaded()
{
	ui->initKeyValue(keyModel);

	setObjectName("valueTabReady");
}

void ValueTab::renameKey()
{
	ui->showLoader();
	ui->renameKey->setEnabled(false);
	keyModel->renameKey(ui->keyName->text());	
}

void ValueTab::keyRenamed()
{
	key->setText(ui->keyName->text());
	ui->renameKey->setEnabled(true);
	ui->hideLoader();
}

void ValueTab::deleteKey()
{
	ui->showLoader();
	keyModel->deleteKey();	
}

void ValueTab::keyDeleted()
{
	key->remove();
	ui->hideLoader();
	emit keyDeleted(this, key);	
}

void ValueTab::updateValue(const QString& value, const QModelIndex *cellIndex)
{
	ui->showLoader();
	keyModel->updateValue(value, cellIndex);
}

void ValueTab::valueUpdated()
{
	ui->hideLoader();
}

ValueTab::~ValueTab()
{
	delete ui;

	keyModel->disconnect();	

	PaginatedModel::delayedDeallocator(keyModel);
	keyModel = nullptr;
}


void ValueTab::errorOccurred(const QString& message)
{
	QMessageBox::warning(this, "Error occurred", message);
}

