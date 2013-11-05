#include <QtXml>

#include "RedisConnectionsManager.h"
#include "RedisConnectionConfig.h"
#include "RedisConnectionOverSsh.h"
#include "RedisConnection.h"
#include "RedisServerItem.h"
#include "RedisServerDbItem.h"

RedisConnectionsManager::RedisConnectionsManager(QString config)
	: configPath(config), connectionSettingsChanged(false), 
	 QStandardItemModel(nullptr)
{
	if (!config.isEmpty() && QFile::exists(config)) {
		LoadConnectionsConfigFromFile(config);
	}

	connectionsThread.start();
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
	c->moveToThread(&connectionsThread);
	connect(&connectionsThread, &QThread::finished, c, &QObject::deleteLater);

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
	if (LoadConnectionsConfigFromFile(path, true)) {
		return true;
	}

	return false;
}


bool RedisConnectionsManager::LoadConnectionsConfigFromFile(QString& config, bool saveChangesToFile)
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

	if (!saveChangesToFile)
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

void RedisConnectionsManager::setFilter(QRegExp & pattern)
{
	if (pattern.isEmpty()) {
		return;
	}

	filter = pattern;

	updateFilter();
}

void RedisConnectionsManager::resetFilter()
{
	filter = QRegExp("");

	updateFilter();
}

void RedisConnectionsManager::updateFilter()
{
	int rowsCount, childRowsCount;
	RedisServerDbItem * db;
	QStandardItem * element;

	rowsCount = rowCount();

	for (int i = 0; i < rowsCount; i++) {
		element = item(i);

		childRowsCount  = element->rowCount();

		for (int childRowIndex = 0; childRowIndex < childRowsCount; childRowIndex++) 
		{
			db = (RedisServerDbItem *) element->child(childRowIndex);

			db->setFilter(filter);
		}
	}
}

