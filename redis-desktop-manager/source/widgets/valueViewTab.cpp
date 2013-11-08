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
}



void valueViewTab::init()
{
	type = key->getKeyType();	

	switch (type)
	{
	case RedisKeyItem::String:	

		uiString = new Ui::stringViewTab;
		initUI(uiString, key->text());	

		uiString->keyValue->setPlainText(key->getValue().toString());

		break;

	case RedisKeyItem::Hash:

		uiHash = new Ui::hashViewTab;
		initUI(uiHash, key->text());		

		model = new HashKeyModel(key->getValue().toStringList());
		initValueView(uiHash, model);

		break;

	case RedisKeyItem::List:		
	case RedisKeyItem::Set:
		uiList = new Ui::listViewTab;
		initUI(uiList, key->text());

		model = new ListKeyModel(key->getValue().toStringList());
		initValueView(uiList, model);

		break;

	case RedisKeyItem::ZSet:
		uiZSet = new Ui::zsetViewTab;
		initUI(uiZSet, key->text());

		model = new SortedSetKeyModel(key->getValue().toStringList());
		initValueView(uiZSet, model);

		break;	
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

