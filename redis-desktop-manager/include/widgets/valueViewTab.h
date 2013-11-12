#ifndef HASHVIEWTAB_H
#define HASHVIEWTAB_H

#include <QWidget>
#include <QStandardItemModel>
#include "RedisKeyItem.h"

class ValueTabView;

class ValueTab : public QWidget
{
	Q_OBJECT

public:
	ValueTab(RedisKeyItem * key);
	~ValueTab();

protected:	
	RedisKeyItem * key;
	RedisKeyItem::Type type;
	QStandardItemModel * model;
	ValueTabView * ui;

	void init();

	QStandardItemModel * getModelForKey(RedisKeyItem::Type, const QVariant&);

protected slots:
	void valueLoaded(const QVariant&, QObject *);
	//void error(QString&);
};

#endif // HASHVIEWTAB_H
