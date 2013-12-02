#include "PaginatedModel.h"

#include <QtConcurrent>

PaginatedModel::PaginatedModel()
	: currentPage(0)
{
	rawData = new QStringList;
}

int PaginatedModel::getCurrentPage()
{
	return currentPage;
}

int PaginatedModel::itemsCount()
{
	return rawData->size();
}

int PaginatedModel::getPagesCount()
{
	int pages = itemsCount() / itemsOnPageLimit;

	if (itemsCount() % itemsOnPageLimit > 0) {
		pages++;
	}

	return pages;
}

PaginatedModel::~PaginatedModel()
{
	if (rawData == nullptr)
		return;

	QtConcurrent::run(delayedDeallocator, rawData);	
}

void PaginatedModel::delayedDeallocator(QStringList *object)
{
	delete object;
}
