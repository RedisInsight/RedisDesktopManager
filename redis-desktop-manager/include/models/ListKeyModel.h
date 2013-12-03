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

protected:
	void initModel(const QVariant &);

};

#endif // LISTKEYMODEL_H
