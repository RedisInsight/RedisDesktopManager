#pragma once

#include <QtCore>
#include <QStandardItemModel>
#include "RedisConnectionAbstract.h"

class TestRedisConnectionsManager;

class RedisConnectionsManager : public QStandardItemModel
{
	friend class TestRedisConnectionsManager;

public:
	RedisConnectionsManager(QString);
	~RedisConnectionsManager(void);

	void AddConnection(RedisConnectionAbstract *);

private:
	QString configPath;
	QList<RedisConnectionAbstract *> connections;
	bool connectionSettingsChanged;

protected:
	void LoadConnectionsConfigFromFile(QString);
	void SaveConnectionsConfigToFile(QString);
	void loadKeys(RedisConnectionAbstract *);
};

