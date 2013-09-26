#include "RedisConnectionsModel.h"

RedisConnectionsModel::RedisConnectionsModel(QObject *parent)
	: QStandardItemModel(parent)
{

}

RedisConnectionsModel::~RedisConnectionsModel()
{

}


void RedisConnectionsModel::loadKeys(RedisConnectionAbstract * connection)
{
	if (!connection->isConnected()) {
		connection->connect();
	}

	QVariant result = connection->execute("KEYS *");

	if (result.isNull()) {
		return;
	}

	QStringList keys = result.toStringList();

	if (keys.empty() == true) {
		return;
	}

	QList<QStandardItem *> rootItems = findItems(connection->config.name, Qt::MatchFixedString);
	QStandardItem * rootItem = nullptr;

	if (rootItems.size() == 0) {
		rootItem = new QStandardItem(connection->config.name);	
		rootItem->setIcon(QIcon(":/images/redisIcon.png"));

		appendRow(rootItem);
	} else {
		rootItem = rootItems.first();
	}

	for (QString key : keys) {
		rootItem->appendRow(new QStandardItem(key));
	}
}