#pragma once

#include "ItemWithNaturalSort.h"
#include "ConnectionBridge.h"
#include "RedisServerDbItem.h"

class RedisKeyItem;
class RedisConnectionsManager;

class RedisServerItem : public QObject, public ItemWithNaturalSort
{
	Q_OBJECT

	friend class RedisServerDbItem;
	friend class RedisKeyItem;

public:	
	RedisServerItem(ConnectionBridge * c);

	void runDatabaseLoading();	
	QStringList getInfo();

	void reload();
	void unload();

	ConnectionBridge * getConnection();
	void setConnection(ConnectionBridge * c);

	int virtual type() const;
	
	const static int TYPE = 2000;

private:	
	ConnectionBridge * connection;
	bool isDbInfoLoaded;

	void setBusyIcon();
	void setOfflineIcon();
	void setNormalIcon();
	void getItemNameFromConnection();

private slots:
	void databaseDataLoaded(RedisConnectionAbstract::RedisDatabases);
	void proccessError(QString);

signals:
	void error(QString);
	void databasesLoaded();
	void unlockUI();
};

