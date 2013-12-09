#include "KeyModel.h"

#include <QVariant>
#include "Command.h"
#include "ConnectionBridge.h"

KeyModel::KeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex)
	: db(db), keyName(keyName), dbIndex(dbIndex)
{	
}

QString KeyModel::getKeyName()
{
	return keyName;
}

void KeyModel::loadedValue(Response value)
{
	initModel(value.getValue());

	emit valueLoaded();
}

void KeyModel::renameKey(const QString& newKeyName)
{	
	QString renameCommand = QString("RENAME %1 %2")
								.arg(keyName)
								.arg(newKeyName);
	
	db->addCommand(Command(renameCommand, this, CALLMETHOD("loadedRenameStatus"), dbIndex));
}

void KeyModel::loadedRenameStatus(Response result)
{
	if (result.isErrorMessage()) 
		emit keyRenameError(result.getValue().toString());
	else 
		emit keyRenamed();	
}

void KeyModel::deleteKey()
{
	QString renameCommand = QString("DEL %1")
		.arg(keyName);

	db->addCommand(Command(renameCommand, this, CALLMETHOD("loadedDeleteStatus"), dbIndex));
}

void KeyModel::loadedDeleteStatus(Response result)
{
	if (result.isErrorMessage()) 
	{
		emit keyDeleteError(result.getValue().toString());
	}
	else 
		emit keyDeleted();	
}


KeyModel::~KeyModel()
{
}