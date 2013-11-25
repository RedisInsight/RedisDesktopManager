#ifndef HASHKEYMODEL_H
#define HASHKEYMODEL_H

#include "PaginatedModel.h"

class HashKeyModel : public PaginatedModel
{
public:
	HashKeyModel(QStringList& values);

	void setCurrentPage(int);

	int itemsCount();

};

#endif // HASHKEYMODEL_H
