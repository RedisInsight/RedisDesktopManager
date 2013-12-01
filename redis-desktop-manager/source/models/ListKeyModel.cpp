#include "ListKeyModel.h"

ListKeyModel::ListKeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex)
	: KeyModel(db, keyName, dbIndex), PaginatedModel()
{
	setColumnCount(1);
}

void ListKeyModel::loadValue()
{
	QString command = QString("LRANGE %1 0 -1").arg(keyName);

	db->addCommand(Command(command, this, CALLMETHOD("loadedValue"), dbIndex));
}

void ListKeyModel::setCurrentPage(int page)
{
	if (page == currentPage) {
		return;
	}

	clear();

	QStringList labels("Value");	
	setHorizontalHeaderLabels(labels);

	currentPage = page;

	int size = rawData.size();

	setRowCount( (itemsOnPageLimit > size)? size : itemsOnPageLimit);

	int startShiftPosition = itemsOnPageLimit * (currentPage - 1);
	int limit = startShiftPosition  + itemsOnPageLimit;

	for (int i = startShiftPosition, row = 0; i < limit && i < size; ++i, ++row) {

		QStandardItem * value = new QStandardItem(rawData.at(i));
		setItem(row, 0, value);
	}
}
