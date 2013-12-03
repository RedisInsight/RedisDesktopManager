#ifndef HASHVIEWTAB_H
#define HASHVIEWTAB_H

#include <QWidget>
#include "RedisKeyItem.h"
#include "KeyModel.h"

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

protected slots:
	void keyTypeLoaded(const QVariant & type);

	void valueLoaded();

	void deleteKey();

	void renameKey();

	//void onKeyRenamed();
	//void onKeyRenameError();	

signals:
	void error(const QString&);
};

#endif // HASHVIEWTAB_H
