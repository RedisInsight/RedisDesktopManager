#ifndef REDISKEYITEM_H
#define REDISKEYITEM_H

#include "ItemWithNaturalSort.h"

class RedisServerDbItem;
class KeyModel;

class RedisKeyItem : public ItemWithNaturalSort
{

public:
	const static int TYPE = 2200;

	RedisKeyItem();
	RedisKeyItem(QString name, RedisServerDbItem * db, const QIcon &);		

	QString getTabLabelText();

	int virtual type() const;

	KeyModel * getKeyModel();

	void init(QString name, RedisServerDbItem * db, const QIcon &);

private:	
	RedisServerDbItem * db;	
};

#endif // REDISKEYITEM_H
