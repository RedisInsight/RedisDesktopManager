#pragma once

#include <QStandardItem>

class ItemWithNaturalSort : public QStandardItem
{
public:

	ItemWithNaturalSort();

	ItemWithNaturalSort(const QIcon & icon, const QString & text);	

	bool operator<(const QStandardItem & second) const;
};

