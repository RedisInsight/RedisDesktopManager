#include "valueViewTab.h"
#include "HashKeyModel.h"
#include "RedisKeyItem.h"
#include "ListKeyModel.h"
#include "SortedSetKeyModel.h"


valueViewTab::valueViewTab(RedisKeyItem * key)	
	: key(key), uiString(nullptr), uiHash(nullptr), 
	  uiList(nullptr), uiZSet(nullptr)
{	
	init();

	connect(key, SIGNAL(valueLoaded(const QVariant&, QObject *)), this, SLOT(valueLoaded(const QVariant&, QObject *)));

	key->getValue();
}

void valueViewTab::init()
{
	type = key->getKeyType();	

	switch (type)
	{
	case RedisKeyItem::String:	
		uiString = new Ui::stringViewTab;
		initUI(uiString, key->text());	
		break;

	case RedisKeyItem::Hash:
		uiHash = new Ui::hashViewTab;
		initUI(uiHash, key->text());
		break;

	case RedisKeyItem::List:		
	case RedisKeyItem::Set:
		uiList = new Ui::listViewTab;
		initUI(uiList, key->text());
		break;

	case RedisKeyItem::ZSet:
		uiZSet = new Ui::zsetViewTab;
		initUI(uiZSet, key->text());
		break;	
	}
}

void valueViewTab::valueLoaded(const QVariant& value, QObject * owner)
{
	if (owner != key) {
		return;
	}

	switch (type)
	{
	case RedisKeyItem::String:	
		uiString->keyValue->setPlainText(value.toString());
		return;		

	case RedisKeyItem::Hash:
		model = new HashKeyModel(value.toStringList());
		initValueView(uiHash, model);
		return;		

	case RedisKeyItem::List:		
	case RedisKeyItem::Set:
		model = new ListKeyModel(value.toStringList());
		initValueView(uiList, model);
		return;

	case RedisKeyItem::ZSet:		
		model = new SortedSetKeyModel(value.toStringList());
		initValueView(uiZSet, model);
		return;	
	}
}

valueViewTab::~valueViewTab()
{
	switch (type)
	{
	case RedisKeyItem::String:						
		delete uiString; 		
		break;

	case RedisKeyItem::Hash:		
		delete uiHash;		
		break;

	case RedisKeyItem::List:		
	case RedisKeyItem::Set:
		delete uiList;
		break;

	case RedisKeyItem::ZSet:
		delete uiZSet;
		break;	
	}
}




