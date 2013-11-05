#ifndef HASHVIEWTAB_H
#define HASHVIEWTAB_H

#include <QWidget>
#include "ui_hashViewTab.h"


class RedisKeyItem;

class hashViewTab : public QWidget
{
	Q_OBJECT

public:
	hashViewTab(RedisKeyItem * key);

private:
	Ui::hashViewTab ui;
};

#endif // HASHVIEWTAB_H
