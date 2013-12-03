#pragma once

#include <QStringList>
#include "KeyModel.h"

class PaginatedModel : public KeyModel
{
	Q_OBJECT

public:
	PaginatedModel(ConnectionBridge * db, const QString &keyName, int dbIndex);

	virtual ~PaginatedModel();

	virtual int getCurrentPage();

	virtual void setCurrentPage(int) = 0;

	virtual int itemsCount();

	virtual int getPagesCount();

protected:
	QStringList * rawData;
	int currentPage;
	static const int itemsOnPageLimit = 500;

	static void delayedDeallocator(QStringList *);
};

