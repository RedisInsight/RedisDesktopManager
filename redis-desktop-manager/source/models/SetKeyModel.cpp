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