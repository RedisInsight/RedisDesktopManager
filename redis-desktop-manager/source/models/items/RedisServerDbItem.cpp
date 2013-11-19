
#include "RedisServerDbItem.h"

#include "RedisServerItem.h"
#include "RedisKeyItem.h"
#include "RedisKeyNamespace.h"
#include "ItemWithNaturalSort.h"
#include "KeysTreeRenderer.h"

RedisServerDbItem::RedisServerDbItem(QString name, int keysCount, RedisServerItem * parent) 
	: server(parent), isKeysLoaded(false), dbIndex(0), keysCount(keysCount), name(name), 
	  currentKeysPoolPosition(0), iconStorage(QIcon(":/images/key.png"), QIcon(":/images/namespace.png"))
{	
	setNormalIcon();
	setText(QString("%1 (%2)").arg(name).arg(keysCount));

	QRegExp getDbIndex("(\\d+)");		

	if (getDbIndex.indexIn(name) > -1) {				
		dbIndex = getDbIndex.cap(1).toInt();
	}

	setEditable(false);	


	connect(&keysLoadingWatcher, SIGNAL(finished()), this, SLOT(keysLoadingFinished()));
}

void RedisServerDbItem::loadKeys()
{
	if (isKeysLoaded) return;

	setBusyIcon();	

	//wait for signal from connection	
	connect(server->connection, SIGNAL(error(QString)), this, SLOT(proccessError(QString)));
	connect(server->connection, SIGNAL(responseResieved(const QVariant &, QObject *)),
		this, SLOT(keysLoaded(const QVariant &, QObject *)));
	connect(server->connection, SIGNAL(operationProgress(int, QObject *)), 
		this, SLOT(keysLoadingStatusChanged(int, QObject *)));
	
	server->connection->addCommand(Command("keys *", this, dbIndex));
}

void RedisServerDbItem::keysLoaded(const QVariant &keys, QObject *owner)
{
	if (owner != this) {
		return;
	}

	server->connection->disconnect(this);

	rawKeys = keys.toStringList();

	int resultSize = rawKeys.size();

	if (resultSize == 0) {
		server->unlockUI();
		setNormalIcon();
		return;
	}

	if (resultSize < keysCount) {
		server->error(QString("Loaded keys: %2 of %3. Error - %4 <br /> Check <a href='https://github.com/uglide/RedisDesktopManager/wiki/Known-issues'>documentation</a>")
			.arg(resultSize)
			.arg(keysCount)
			.arg(server->connection->getLastError()));
	}


	renderKeys(rawKeys);

	setNormalIcon();
	isKeysLoaded = true;	
}

void RedisServerDbItem::proccessError(QString srcError)
{
	server->connection->disconnect(this);
	setNormalIcon();

	QString message = QString("Can not load keys. %1")
		.arg(srcError);

	emit server->error(message);
}

void RedisServerDbItem::setFilter(QRegExp &pattern)
{
	filter = pattern;

	removeRows(0, rowCount());

	renderKeys(rawKeys);

	keysPool = nullptr;
}

void RedisServerDbItem::resetFilter()
{
	filter.setPattern("");

	removeRows(0, rowCount());

	renderKeys(rawKeys);
}

void RedisServerDbItem::renderKeys(QStringList &rawKeys)
{
	if (rawKeys.size() == 0) 
		return;

	keysLoadingResult = QtConcurrent::run(KeysTreeRenderer::renderKeys, this, rawKeys, filter, iconStorage);

	keysLoadingWatcher.setFuture(keysLoadingResult);

	server->statusMessage(QString("Keys rendering ..."));
}

void RedisServerDbItem::setBusyIcon()
{
	setIcon(QIcon(":/images/wait.png"));
}

void RedisServerDbItem::setNormalIcon()
{
	setIcon(QIcon(":/images/db.png"));
}

int RedisServerDbItem::type() const
{
	return TYPE;
}

bool RedisServerDbItem::operator<(const QStandardItem & other) const
{
 	if (other.type() == TYPE) {
		const RedisServerDbItem * another = dynamic_cast<const RedisServerDbItem *>(&other); 
 	}	

 	return this->text() < other.text();
}

int RedisServerDbItem::getDbIndex() const
{
	return dbIndex;
}

void RedisServerDbItem::keysLoadingStatusChanged(int progressValue, QObject * owner)
{
	if (owner != this) {
		return;
	}
	server->statusMessage(
		QString("Downloading keys list from database: %1 / %2 ").arg(progressValue).arg(keysCount)
		);
}

void RedisServerDbItem::keysLoadingFinished()
{
	appendRows(keysLoadingResult.result());
	server->statusMessage(QString("Keys rendering done"));
	server->unlockUI();
}

RedisServerDbItem::~RedisServerDbItem()
{
}