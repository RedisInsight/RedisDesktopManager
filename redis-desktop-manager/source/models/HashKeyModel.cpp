#include "HashKeyModel.h"
#include <QStandardItem>

HashKeyModel::HashKeyModel(QStringList& values)
	: QStandardItemModel(nullptr), PaginatedModel(values)
{
	QStringList labels;
	labels << "Hash Key" << "Hash Value";
	setHorizontalHeaderLabels(labels);

	setColumnCount(2);
	setCurrentPage(1);
}


void HashKeyModel::setCurrentPage(int page)
{
	if (page == currentPage) {
		return;
	}

	currentPage = page;

	int size = rawData.size();

	setRowCount( (itemsOnPageLimit > size)? size : itemsOnPageLimit);
	
	int startShiftPosition = itemsOnPageLimit * 2 * (currentPage - 1);
	int limit = startShiftPosition + itemsOnPageLimit * 2;

	for (int i = startShiftPosition, row = 0; i < limit && i < size; ++i, ++row) {

		QStandardItem * key = new QStandardItem(rawData.at(i));
		QStandardItem * value = new QStandardItem(rawData.at(++i));
		setItem(row, 0, key);
		setItem(row, 1, value);
	}
}
