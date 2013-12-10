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

void StringKeyModel::updateValue(const QString& value, const QModelIndex *cellIndex)
{
    Q_UNUSED(cellIndex);

	QString updateCommand = QString("SET %1 %2")
		.arg(keyName)
		.arg(value);

	db->addCommand(Command(updateCommand, this, CALLMETHOD("loadedUpdateStatus"), dbIndex));
}

void StringKeyModel::loadedUpdateStatus(Response result)
{
	if (result.isErrorMessage()) 
	{
		emit valueUpdateError(result.getValue().toString());
	}
	else 
		emit valueUpdated();	
}
