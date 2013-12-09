#ifndef LISTKEYMODEL_H
#define LISTKEYMODEL_H

#include "KeyModel.h"
#include "PaginatedModel.h"

class ListKeyModel : public PaginatedModel
{
	Q_OBJECT

public:
	ListKeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex);

	void setCurrentPage(int page);

	void loadValue();

	void updateValue(const QString& value, const QModelIndex *cellIndex);

protected slots:
	void loadedUpdateStatus(Response);
};

#endif // LISTKEYMODEL_H
