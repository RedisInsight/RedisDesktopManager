#ifndef HASHVIEWTAB_H
#define HASHVIEWTAB_H

#include <QWidget>
#include "PaginatedModel.h"
#include "RedisKeyItem.h"
#include "KeyModel.h"

class ValueTabView;

class ValueTab : public QWidget
{
	Q_OBJECT

public:
	ValueTab(RedisKeyItem * key);
	~ValueTab();

	static void delayedDeallocator(QObject *);

protected:	
	KeyModel * keyModel;
	KeyModel::Type type;
	PaginatedModel * model;
	ValueTabView * ui;

	void initPagination();

	PaginatedModel * getModelForKey(KeyModel::Type, const QVariant&);	

protected slots:
	void keyTypeLoaded(KeyModel::Type);
	void valueLoaded(const QVariant&, QObject *);
	void loadNextPage();
	void loadPreviousPage();

	//void error(QString&);
};

#endif // HASHVIEWTAB_H
