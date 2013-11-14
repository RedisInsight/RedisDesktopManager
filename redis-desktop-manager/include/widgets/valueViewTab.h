#ifndef HASHVIEWTAB_H
#define HASHVIEWTAB_H

#include <QWidget>
#include "PaginatedModel.h"
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
	PaginatedModel * model;
	ValueTabView * ui;

	void init();

	void initPagination();

	PaginatedModel * getModelForKey(RedisKeyItem::Type, const QVariant&);	

protected slots:
	void valueLoaded(const QVariant&, QObject *);
	void loadNextPage();
	void loadPreviousPage();

	//void error(QString&);
};

#endif // HASHVIEWTAB_H
