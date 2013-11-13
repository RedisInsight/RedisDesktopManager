#ifndef SORTEDSETKEYMODEL_H
#define SORTEDSETKEYMODEL_H

#include <QStandardItemModel>
#include "PaginatedModel.h"

class SortedSetKeyModel : public QStandardItemModel, public PaginatedModel
{
public:
	SortedSetKeyModel(QStringList& values);

	void setCurrentPage(int page);
};

#endif // SORTEDSETKEYMODEL_H
