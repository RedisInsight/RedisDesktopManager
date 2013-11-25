#ifndef HASHVIEWTAB_H
#define HASHVIEWTAB_H

#include <QWidget>
#include "PaginatedModel.h"
#include "RedisKeyItem.h"
#include "KeyModel.h"
#include "AbstractFormatter.h"

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

	AbstractFormatter::FormatterType currentFormatter;
	AbstractFormatter * formatter;

	const QModelIndex * currentCell;

	QString jsonValueFormatter(const QString&);

	void initPagination();

	PaginatedModel * getModelForKey(KeyModel::Type, const QVariant&);	

protected slots:
	void keyTypeLoaded(KeyModel::Type);
	void valueLoaded(const QVariant&, QObject *);
	void loadNextPage();
	void loadPreviousPage();
	void onSelectedItemChanged(const QModelIndex & current, const QModelIndex & previous);
	void currentFormatterChanged(int);

	//void error(QString&);

private:
	QString rawStringValue;
};

#endif // HASHVIEWTAB_H
