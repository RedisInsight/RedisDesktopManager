#include "StringKeyModel.h"

StringKeyModel::StringKeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex)
	: KeyModel(db, keyName, dbIndex)
{
}

void StringKeyModel::loadValue()
{
	QString command = QString("get %1").arg(keyName);

	db->addCommand(Command(command, this, CALLMETHOD("loadedValue"), dbIndex));
}

QString StringKeyModel::getValue()
{
	return plainData;
}

void StringKeyModel::initModel(const QVariant &value)
{
	plainData = value.toString();
}