#include "HashKeyModel.h"
#include <QStandardItem>

HashKeyModel::HashKeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex)
	: KeyModel(db, keyName, dbIndex), PaginatedModel()
{
	setColumnCount(2);
}

void HashKeyModel::loadValue()
{
	QString command = QString("hgetall %1").arg(keyName);

	db->addCommand(Command(command, this, CALLMETHOD("loadedValue"), dbIndex));
}


void HashKeyModel::setCurrentPage(int page)
{
	if (page == currentPage) {
		return;
	}

	clear();

	QStringList labels;
	labels << "Hash Key" << "Hash Value";
	setHorizontalHeaderLabels(labels);

	currentPage = page;

	int size = rawData.size();

	setRowCount( (itemsOnPageLimit > size / 2)? size / 2 : itemsOnPageLimit);
	
	int startShiftPosition = itemsOnPageLimit * 2 * (currentPage - 1);
	int limit = startShiftPosition + itemsOnPageLimit * 2;

	for (int i = startShiftPosition, row = 0; i < limit && i < size; ++i, ++row) {

		QStandardItem * key = new QStandardItem(rawData.at(i));
		QStandardItem * value = new QStandardItem(rawData.at(++i));
		setItem(row, 0, key);
		setItem(row, 1, value);
	}
}

int HashKeyModel::itemsCount()
{
	return rawData.size() / 2;
}