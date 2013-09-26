#include "RedisServerDbItem.h"
#include "RedisServerItem.h"
#include "RedisKeyItem.h"
#include "RedisKeyNamespace.h"

RedisServerDbItem::RedisServerDbItem(QString name, int keysCount, RedisServerItem * parent) 
	: server(parent), isKeysLoaded(false), dbIndex(0)
{	
	setNormalIcon();
	setText(QString("%1 (%2)").arg(name).arg(keysCount));

	QRegExp getDbIndex("(\\d+)");		

	if (getDbIndex.indexIn(name) > -1) {				
		dbIndex = getDbIndex.cap(1).toInt();
	}

	setEditable(false);
}

void RedisServerDbItem::setCurrent()
{
	if (!server->connection->isConnected() 
		&& !server->connection->connect()) {
			return;
	}

	server->connection->selectDb(dbIndex);
}

void RedisServerDbItem::loadKeys()
{
	if (isKeysLoaded) return;

	if (!server->connection->isConnected() 
		&& !server->connection->connect()) {
			return;
	}

	setBusyIcon();

	server->connection->selectDb(dbIndex);

	QStringList rawKeys =  server->connection->getKeys();

	if (rawKeys.size() == 0) {
		setNormalIcon();
		return;
	}

	for (QString rawKey : rawKeys) {
		renderNamaspacedKey(this, rawKey, rawKey);
	}

	setNormalIcon();
	isKeysLoaded = true;
}

void RedisServerDbItem::renderNamaspacedKey(QStandardItem * currItem, 
											QString notProcessedKeyPart, QString fullKey)
{
	if (!notProcessedKeyPart.contains(":")) {
		QStandardItem * newKey = new RedisKeyItem(fullKey, this);
		currItem->appendRow(newKey);	
		return;
	}

	int indexOfNaspaceSeparator = notProcessedKeyPart.indexOf(":");

	QString firstNamespaceName = notProcessedKeyPart.mid(0, indexOfNaspaceSeparator);

	QStandardItem * namespaceItem = nullptr;

	for (int i=0; i < currItem->rowCount(); ++i)
	{
		QStandardItem * child = currItem->child(i);

		if (child->type() == RedisKeyNamespace::TYPE 
			&& child->text() == firstNamespaceName) {
				namespaceItem = child;
				break;
		}
	}

	if (namespaceItem == nullptr) {
		namespaceItem = new RedisKeyNamespace(firstNamespaceName);
		currItem->appendRow(namespaceItem);
	}

	renderNamaspacedKey(namespaceItem, notProcessedKeyPart.mid(indexOfNaspaceSeparator+1), fullKey);	
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

		return dbIndex < another->getDbIndex();
	}	

	return this->text() < other.text();
}

int RedisServerDbItem::getDbIndex() const
{
	return dbIndex;
}