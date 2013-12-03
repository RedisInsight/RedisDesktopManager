#ifndef SORTEDSETKEYMODEL_H
#define SORTEDSETKEYMODEL_H

#include "KeyModel.h"
#include "PaginatedModel.h"

class SortedSetKeyModel : public PaginatedModel
{
public:
	SortedSetKeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex);

	void setCurrentPage(int page);

	int itemsCount();

	void loadValue();
};

#endif // SORTEDSETKEYMODEL_H
