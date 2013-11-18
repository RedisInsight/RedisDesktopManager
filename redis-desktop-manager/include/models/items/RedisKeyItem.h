#ifndef REDISKEYITEM_H
#define REDISKEYITEM_H

#include "ItemWithNaturalSort.h"

class RedisServerDbItem;

class RedisKeyItem : public QObject, public ItemWithNaturalSort
{
	Q_OBJECT

public:

	enum Type {String, Hash, List, Set, ZSet, None, Empty};

	const static int TYPE = 2200;

	RedisKeyItem(QString name, RedisServerDbItem * db, const QIcon &);		

	Type getKeyType();

	void getValue();

	QString getFullText();

	QString getFullName();

	int virtual type() const;	

signals:
	void valueLoaded(const QVariant&, QObject *);
	void keyTypeLoaded();

private:
	Type keyType;
	QString fullName;
	RedisServerDbItem * db;

private slots:
	void loadedValue(const QVariant&, QObject *);
	void loadedType(const QVariant&, QObject *);	
};

#endif // REDISKEYITEM_H
