#ifndef HASHVIEWTAB_H
#define HASHVIEWTAB_H

#include <QWidget>
#include "RedisKeyItem.h"
#include "KeyModel.h"
#include "AbstractFormatter.h"

class ValueTabView;

class ValueTab : public QWidget
{
	Q_OBJECT

public:
	ValueTab(RedisKeyItem * key);

protected:	
	RedisKeyItem * key;

	KeyModel * keyModel;

	ValueTabView * ui;

	AbstractFormatter::FormatterType currentFormatter;

	AbstractFormatter * formatter;

	const QModelIndex * currentCell;	

protected slots:
	void keyTypeLoaded(const QVariant & type);
	void valueLoaded();
	void loadNextPage();
	void loadPreviousPage();
	void onSelectedItemChanged(const QModelIndex & current, const QModelIndex & previous);
	void currentFormatterChanged(int);
	void initPagination();
	void deleteKey();
	void renameKey();

	//void onKeyRenamed();
	//void onKeyRenameError();	

signals:
	void error(const QString&);
};

#endif // HASHVIEWTAB_H
