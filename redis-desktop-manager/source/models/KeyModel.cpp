#include "KeyModel.h"

#include <QEventLoop>
#include <QTimer>
#include <QVariant>

#include "Command.h"

KeyModel::KeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex)
	: db(db), keyName(keyName), dbIndex(dbIndex), keyType(Empty)
{
}

KeyModel::~KeyModel(void)
{
}

QString KeyModel::getKeyName()
{
	return keyName;
}

KeyModel::Type KeyModel::getKeyType()
{
	if (keyType != Empty) {
		return keyType;
	}

	QEventLoop loop;
	QTimer timer;

	timer.setSingleShot(true);
	connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
	connect(this, SIGNAL(keyTypeLoaded()), &loop, SLOT(quit()));
	connect(db, SIGNAL(responseResieved(const QVariant&, QObject *)),
		this, SLOT(loadedType(const QVariant&, QObject*)));

	timer.start(db->getConfig().executeTimeout);
	db->addCommand(Command(QString("type %1").arg(keyName), this, dbIndex));
	loop.exec();

	return keyType;
}

void KeyModel::getValue()
{
	if (keyType == Empty) {
		getKeyType();
	}

	QString command;

	switch (keyType)
	{
	case KeyModel::String:
		command = QString("get %1").arg(keyName);
		break;

	case KeyModel::Hash:		
		command = QString("hgetall %1").arg(keyName);
		break;

	case KeyModel::List:
		command = QString("LRANGE %1 0 -1").arg(keyName);		
		break;

	case KeyModel::Set:
		command = QString("SMEMBERS %1").arg(keyName);				
		break;

	case KeyModel::ZSet:		
		command = QString("ZRANGE %1 0 -1 WITHSCORES").arg(keyName);
		break;
	}	

	if (command.isEmpty()) {
		emit valueLoaded(QVariant(), this);
		return;
	} else {
		connect(db, SIGNAL(responseResieved(const QVariant&, QObject *)),
			this, SIGNAL(valueLoaded(const QVariant&, QObject*)));

		db->addCommand(Command(command, this, dbIndex));
	}
}

void KeyModel::loadedValue(const QVariant& value, QObject *sender)
{
	if (sender != this) {
		return;
	}

	db->disconnect(this);

	emit valueLoaded(value, this);
}

void KeyModel::loadedType(const QVariant& result, QObject * owner)
{
	if (owner != this) {
		return;
	}

	db->disconnect(this);

	QString t = result.toString();

	keyType = None;

	if (t == "string")
		keyType = String;

	if (t == "hash") 
		keyType = Hash;

	if (t == "list")
		keyType = List;

	if (t == "set") 
		keyType = Set;

	if (t == "zset") 
		keyType = ZSet;

	emit keyTypeLoaded();

	return;
}