#include <QtXml>

#include "RedisConnectionsManager.h"
#include "RedisConnectionConfig.h"
#include "RedisConnectionOverSsh.h"
#include "RedisConnection.h"
#include "RedisServerItem.h"

RedisConnectionsManager::RedisConnectionsManager(QString config)
	: configPath(config), connectionSettingsChanged(false), QStandardItemModel(nullptr)
{
	if (!config.isEmpty() && QFile::exists(config)) {
		LoadConnectionsConfigFromFile(config);
	}
}


RedisConnectionsManager::~RedisConnectionsManager(void)
{
	if (connectionSettingsChanged) {
		SaveConnectionsConfigToFile(configPath);
	}
}

void RedisConnectionsManager::AddConnection(RedisConnectionAbstract * c)
{
	//add connection to internal container
	connections.push_back(c);

	//add connection to view container	
	appendRow(new RedisServerItem(c));

	//mark settings as unsaved
	connectionSettingsChanged = true;
}

bool RedisConnectionsManager::RemoveConnection(RedisServerItem * c)
{
	if (c == nullptr) {
		return false;
	}

	bool removedFromContainer = connections.removeOne(c->getConnection());

	bool removedFromModel = this->removeRow(c->row());

	//mark settings as unsaved
	if (removedFromContainer && removedFromModel) 
		connectionSettingsChanged = true;

	return removedFromContainer && removedFromModel;
}

void RedisConnectionsManager::UpdateConnection(RedisConnectionAbstract * old, RedisConnectionAbstract * newConnection) 
{
	connections.removeOne(old);
	connections.push_back(newConnection);

	connectionSettingsChanged = true;
}

bool RedisConnectionsManager::ImportConnections(QString &path)
{
	if (LoadConnectionsConfigFromFile(path)) {
		connectionSettingsChanged = true;
		return true;
	}

	return false;
}


bool RedisConnectionsManager::LoadConnectionsConfigFromFile(QString& config)
{
	QFile conf(config);
	
	if (!conf.open(QIODevice::ReadOnly)) 
		return false;	
	
	QDomDocument xmlConf;

	if (xmlConf.setContent(&conf)) {

		QDomNodeList connectionsList = xmlConf.elementsByTagName("connection");
		
		for (int i = 0; i < connectionsList.size(); ++i) {

			QDomNode connection = connectionsList.at(i);

			if (connection.nodeName() != "connection") continue;

			RedisConnectionConfig conf = RedisConnectionConfig::createFromXml(connection);

			if (conf.isNull()) continue;

			RedisConnectionAbstract * c;

			if (conf.useSshTunnel()) {
				c = new RedisConnectionOverSsh(conf);
			} else {
				c = new RedisConnection(conf);
			}

			AddConnection(c);
		}		
	}
	conf.close();

	connectionSettingsChanged = false;	

	return true;
}

void RedisConnectionsManager::SaveConnectionsConfigToFile(QString pathToFile)
{
	QDomDocument config;

	QDomProcessingInstruction xmlProcessingInstruction = config.createProcessingInstruction("xml", "version=\"1.0\"");
	config.appendChild(xmlProcessingInstruction);

	QDomElement connectionsItem = config.createElement("connections");

	config.appendChild(connectionsItem);

	for (RedisConnectionAbstract * c : connections) {
		connectionsItem.appendChild(c->config.toXml(config));
	}

	QFile confFile(configPath);

	if (confFile.open(QIODevice::WriteOnly)) {
		QTextStream(&confFile) << config.toString();
		confFile.close();
	}

	return;
}

void RedisConnectionsManager::loadKeys(RedisConnectionAbstract * connection)
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
