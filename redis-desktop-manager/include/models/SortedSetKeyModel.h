#ifndef SORTEDSETKEYMODEL_H
#define SORTEDSETKEYMODEL_H

#include "PaginatedModel.h"

class SortedSetKeyModel : public PaginatedModel
{
public:
	SortedSetKeyModel(QStringList& values);

	void setCurrentPage(int page);

	int itemsCount();
};

#endif // SORTEDSETKEYMODEL_H
