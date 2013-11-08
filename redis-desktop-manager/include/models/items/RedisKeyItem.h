#ifndef REDISKEYITEM_H
#define REDISKEYITEM_H

#include <QStandardItem>

class RedisServerDbItem;

class RedisKeyItem : public QObject, public QStandardItem
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
	void valueLoaded(QVariant&);

private:
	Type keyType;
	QString fullName;
	RedisServerDbItem * db;
};

#endif // REDISKEYITEM_H
