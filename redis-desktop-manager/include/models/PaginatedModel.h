#pragma once

#include <QStringList>

class PaginatedModel
{
public:
	PaginatedModel(QStringList&);

	virtual int getCurrentPage();

	virtual void setCurrentPage(int) = 0;

	virtual int itemsCount();

protected:
	QStringList rawData;
	int currentPage;
	static const int itemsOnPageLimit = 500;
};

