#pragma once

#include <QtCore>
#include <QThread>
#include <QStandardItemModel>
#include "RedisConnectionAbstract.h"
#include "ConnectionBridge.h"

class TestRedisConnectionsManager;
class RedisServerItem;

class RedisConnectionsManager : public QStandardItemModel
{
	Q_OBJECT

	friend class TestRedisConnectionsManager;

public:
	RedisConnectionsManager(QString, QObject *);
	~RedisConnectionsManager(void);

	void AddConnection(ConnectionBridge *);	
	bool RemoveConnection(RedisServerItem *);
	bool ImportConnections(QString &);
	bool SaveConnectionsConfigToFile(QString);

	void setFilter(QRegExp &);
	void resetFilter();

private:
	QThread connectionsThread;

	QString configPath;
	QList<ConnectionBridge *> connections;
	bool connectionSettingsChanged;
	QRegExp filter;

protected:
	bool LoadConnectionsConfigFromFile(QString& config, bool saveChangesToFile = false);	

protected slots:
	void updateFilter();
};

