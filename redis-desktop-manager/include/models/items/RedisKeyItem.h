#ifndef REDISKEYITEM_H
#define REDISKEYITEM_H

#include <QStandardItem>

class RedisServerDbItem;

class RedisKeyItem : public QStandardItem
{
public:

	enum Type {String, Hash, List, Set, ZSet, None, Empty};

	const static int TYPE = 2200;

	RedisKeyItem(QString name, RedisServerDbItem * db, const QIcon &);		

	Type getKeyType();

	QVariant getValue();

	QString getFullText();

	QString getFullName();

	int virtual type() const;	

private:
	Type keyType;
	QString fullName;
	RedisServerDbItem * db;
};

#endif // REDISKEYITEM_H
