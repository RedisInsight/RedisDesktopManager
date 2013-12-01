#include "PaginatedModel.h"


PaginatedModel::PaginatedModel()
	: currentPage(0)
{
}

int PaginatedModel::getCurrentPage()
{
	return currentPage;
}

int PaginatedModel::itemsCount()
{
	return rawData.size();
}

int PaginatedModel::getPagesCount()
{
	int pages = itemsCount() / itemsOnPageLimit;

	if (itemsCount() % itemsOnPageLimit > 0) {
		pages++;
	}

	return pages;
}