#pragma once

#include <QtCore>
#include <QStandardItemModel>
#include "RedisConnectionAbstract.h"

class TestRedisConnectionsManager;
class RedisServerItem;

class RedisConnectionsManager : public QStandardItemModel
{
	friend class TestRedisConnectionsManager;

public:
	RedisConnectionsManager(QString);
	~RedisConnectionsManager(void);

	void AddConnection(RedisConnectionAbstract *);
	void UpdateConnection(RedisConnectionAbstract * old, RedisConnectionAbstract * newConnection);
	bool RemoveConnection(RedisServerItem *);
	bool ImportConnections(QString &);

	void setFilter(QRegExp &);
	void updateFilter();
	void resetFilter();

private:
	QString configPath;
	QList<RedisConnectionAbstract *> connections;
	bool connectionSettingsChanged;
	QRegExp filter;

protected:
	bool LoadConnectionsConfigFromFile(QString& config, bool saveChangesToFile = false);
	void SaveConnectionsConfigToFile(QString);
};

