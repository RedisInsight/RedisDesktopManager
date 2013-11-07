#pragma once

#include <QtCore>
#include <QThread>
#include <QStandardItemModel>
#include "RedisConnectionAbstract.h"

class TestRedisConnectionsManager;
class RedisServerItem;

class RedisConnectionsManager : public QStandardItemModel
{
	Q_OBJECT

	friend class TestRedisConnectionsManager;

public:
	RedisConnectionsManager(QString, QObject *);
	~RedisConnectionsManager(void);

	void AddConnection(RedisConnectionAbstract *);
	void UpdateConnection(RedisConnectionAbstract * old, RedisConnectionAbstract * newConnection);
	bool RemoveConnection(RedisServerItem *);
	bool ImportConnections(QString &);

	void setFilter(QRegExp &);
	void resetFilter();

private:
	QThread connectionsThread;

	QString configPath;
	QList<RedisConnectionAbstract *> connections;
	bool connectionSettingsChanged;
	QRegExp filter;

protected:
	bool LoadConnectionsConfigFromFile(QString& config, bool saveChangesToFile = false);
	void SaveConnectionsConfigToFile(QString);

protected slots:
	void updateFilter();
};

