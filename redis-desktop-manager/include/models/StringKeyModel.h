#pragma once

#include <QObject>
#include "KeyModel.h"

class StringKeyModel : public KeyModel
{
	Q_OBJECT

public:
	StringKeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex);

	void loadValue();

	QString getValue();

	static const int KEY_MODEL_TYPE = 2;

protected:
	QString plainData;

	void initModel(const QVariant &);

};

