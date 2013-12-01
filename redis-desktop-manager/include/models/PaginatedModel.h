#pragma once

#include <QStringList>

class PaginatedModel
{
public:
	PaginatedModel();

	virtual int getCurrentPage();

	virtual void setCurrentPage(int) = 0;

	virtual int itemsCount();

	virtual int getPagesCount();

protected:
	QStringList rawData;
	int currentPage;
	static const int itemsOnPageLimit = 500;
};

