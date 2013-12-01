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
	RedisKeyItem * key;
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
	void keyTypeLoaded(const QVariant & type);
	void valueLoaded(const QVariant&);
	void loadNextPage();
	void loadPreviousPage();
	void onSelectedItemChanged(const QModelIndex & current, const QModelIndex & previous);
	void currentFormatterChanged(int);

	void deleteKey();

	void renameKey();
	//void onKeyRenamed();
	//void onKeyRenameError();

	//void error(QString&);

private:
	QString rawStringValue;
};

#endif // HASHVIEWTAB_H
