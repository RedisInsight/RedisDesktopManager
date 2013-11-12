#include "valueViewTab.h"
#include "HashKeyModel.h"
#include "RedisKeyItem.h"
#include "ListKeyModel.h"
#include "SortedSetKeyModel.h"
#include "ValueTabView.h"

ValueTab::ValueTab(RedisKeyItem * key)	
	: key(key), ui(nullptr), model(nullptr)
{	
	ui = new ValueTabView();

	init();

	connect(key, SIGNAL(valueLoaded(const QVariant&, QObject *)), this, SLOT(valueLoaded(const QVariant&, QObject *)));

	key->getValue();
}

void ValueTab::init()
{
	type = key->getKeyType();		

	if (type == RedisKeyItem::String) {
		ui->init(this, ValueTabView::PlainBased);
	} else {
		ui->init(this);
	}	

	ui->keyName->setText(key->text());	
}

void ValueTab::valueLoaded(const QVariant& value, QObject * owner)
{
	if (owner != key) {
		return;
	}

	if (type == RedisKeyItem::String) {
		ui->setPlainValue(value.toString());
	} else {
		model = getModelForKey(type, value);
		ui->setModel(model);
	}

}

QStandardItemModel * ValueTab::getModelForKey(RedisKeyItem::Type t, const QVariant& val)
{
	switch (type)
	{
	case RedisKeyItem::Hash:		
		return new HashKeyModel(val.toStringList());		

	case RedisKeyItem::List:		
	case RedisKeyItem::Set:
		return new ListKeyModel(val.toStringList());

	case RedisKeyItem::ZSet:		
		return new SortedSetKeyModel(val.toStringList());
	}

	return nullptr;
}


ValueTab::~ValueTab()
{
	if (ui != nullptr) {
		delete ui;
	}

	if (model != nullptr) {
		delete model;
	}
}




