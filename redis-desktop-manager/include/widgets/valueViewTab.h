#ifndef HASHVIEWTAB_H
#define HASHVIEWTAB_H

#include <QWidget>
#include "RedisKeyItem.h"
#include "KeyModel.h"

class ValueTabView;
class Response;

class ValueTab : public QWidget
{
	Q_OBJECT

public:
	ValueTab(RedisKeyItem * key);
	~ValueTab();

protected:	
	RedisKeyItem * key;

	KeyModel * keyModel;

	ValueTabView * ui;

protected slots:
	void keyTypeLoaded(Response type);

	void valueLoaded();

	void deleteKey();

	void renameKey();

	void keyRenamed();

	void keyDeleted();

	void updateValue(const QString&, const QModelIndex *);

	void valueUpdated();

signals:
	void error(const QString&);
	void keyDeleted(QWidget *, RedisKeyItem *);
};

#endif // HASHVIEWTAB_H
