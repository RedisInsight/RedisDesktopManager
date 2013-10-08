#include "HashKeyModel.h"
#include <QStandardItem>

HashKeyModel::HashKeyModel(QStringList values)
	: QStandardItemModel(nullptr)
{
	int size = values.size();

	setRowCount(size / 2);
	setColumnCount(2);

	for (int i = 0, row = 0; i < size; ++i, ++row) {

		QStandardItem * key = new QStandardItem(values.at(i));
		QStandardItem * value = new QStandardItem(values.at(++i));
		setItem(row, 0, key);
		setItem(row, 1, value);
	}

	QStringList labels;
	labels << "Hash Key" << "Hash Value";
	setHorizontalHeaderLabels(labels);

}
