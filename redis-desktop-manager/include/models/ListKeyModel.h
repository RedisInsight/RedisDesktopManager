#ifndef LISTKEYMODEL_H
#define LISTKEYMODEL_H

#include "KeyModel.h"
#include "PaginatedModel.h"

class ListKeyModel : public PaginatedModel
{
public:
	ListKeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex);

	void setCurrentPage(int page);

	void loadValue();

};

#endif // LISTKEYMODEL_H
