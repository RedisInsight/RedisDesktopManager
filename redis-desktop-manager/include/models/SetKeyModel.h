#pragma once

#include "ListKeyModel.h"

class SetKeyModel : public ListKeyModel
{
public:
	SetKeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex);

	void loadValue();
};

