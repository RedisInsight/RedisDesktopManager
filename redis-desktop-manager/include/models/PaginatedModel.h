#pragma once

#include <QStringList>
#include <QStandardItemModel>

class PaginatedModel : public QStandardItemModel
{
public:
	PaginatedModel(QStringList&);

	virtual int getCurrentPage();

	virtual void setCurrentPage(int) = 0;

	virtual int itemsCount();

	virtual int getPagesCount();

protected:
	QStringList rawData;
	int currentPage;
	static const int itemsOnPageLimit = 500;
};

