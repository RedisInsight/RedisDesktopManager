#include "PaginatedModel.h"


PaginatedModel::PaginatedModel(QStringList& values)
	: rawData(values), currentPage(0)
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