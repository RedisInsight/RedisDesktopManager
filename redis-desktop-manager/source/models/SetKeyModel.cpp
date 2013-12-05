#include "SetKeyModel.h"

SetKeyModel::SetKeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex)
	: ListKeyModel(db, keyName, dbIndex)
{
}

void SetKeyModel::loadValue()
{
	QString command = QString("SMEMBERS %1").arg(keyName);

	db->addCommand(Command(command, this, CALLMETHOD("loadedValue"), dbIndex));
}

void SetKeyModel::updateValue(const QString& value, const QModelIndex *cellIndex)
{
	QStandardItem * item = itemFromIndex(*cellIndex);
	QString oldValue = item->text();
	item->setText(value);

	QString deleteOld = QString("SREM %1 %2")
		.arg(keyName)
		.arg(oldValue);

	db->addCommand(Command(deleteOld, this, dbIndex));

	QString addNew = QString("SADD %1 %2")
		.arg(keyName)
		.arg(value);

	db->addCommand(Command(addNew, this, CALLMETHOD("loadedUpdateStatus"), dbIndex));
}

void SetKeyModel::loadedUpdateStatus(Response result)
{
	if (result.isErrorMessage()) 
	{
		emit valueUpdateError(result.getValue().toString());
	}
	else 
	{
		emit valueUpdated();	
	}
}