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

void KeyModel::loadedValue(const QVariant& value)
{
	emit valueLoaded();
}

void KeyModel::renameKey(const QString& newKeyName)
{	
	QString renameCommand = QString("RENAME %1 %2")
								.arg(keyName)
								.arg(newKeyName);
	
	db->addCommand(Command(renameCommand, this, CALLMETHOD("loadedRenameStatus"), dbIndex));
}

void KeyModel::loadedRenameStatus(const QVariant& result)
{
	QString resultString = (result.isNull()) ? "" : result.toString();

	if (resultString.at(0) != 'O')
		emit keyRenameError(resultString);
	else 
		emit keyRenamed();	
}