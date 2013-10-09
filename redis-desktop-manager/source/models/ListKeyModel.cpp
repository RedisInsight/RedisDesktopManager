#include "ListKeyModel.h"

ListKeyModel::ListKeyModel(QStringList values)
	: QStandardItemModel(nullptr)
{

	int size = values.size();

	setRowCount(size);
	setColumnCount(1);

	for (int i = 0, row = 0; i < size; ++i, ++row) {

		QStandardItem * value = new QStandardItem(values.at(i));
		setItem(row, 0, value);
	}

	QStringList labels("Value");	
	setHorizontalHeaderLabels(labels);
}

