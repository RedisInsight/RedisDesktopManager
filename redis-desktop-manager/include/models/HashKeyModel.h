#ifndef HASHKEYMODEL_H
#define HASHKEYMODEL_H

#include "KeyModel.h"
#include "PaginatedModel.h"

class HashKeyModel : public KeyModel, public PaginatedModel
{
	Q_OBJECT

public:
	HashKeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex);

	void setCurrentPage(int);

	int itemsCount();

	void loadValue();

};

#endif // HASHKEYMODEL_H
