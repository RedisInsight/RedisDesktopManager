#pragma once

#include "KeyModel.h"
#include "PaginatedModel.h"

class SetKeyModel : public KeyModel, public PaginatedModel
{
public:
	SetKeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex);

	void setCurrentPage(int page);

	void loadValue();

protected:
	void initModel(const QVariant &);
};

