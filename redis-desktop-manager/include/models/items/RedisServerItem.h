#pragma once

#include <QStandardItem>
#include "RedisConnectionAbstract.h"
#include "RedisServerDbItem.h"

class RedisKeyItem;

class RedisServerItem :  public QStandardItem
{
	friend class RedisServerDbItem;
	friend class RedisKeyItem;

public:	
	RedisServerItem(RedisConnectionAbstract * c);

	bool loadDatabases();	

	void reload();

	int virtual type() const;
	
	const static int TYPE = 2000;

private:	
	RedisConnectionAbstract * connection;
	bool isDbInfoLoaded;

	void setBusyIcon();
	void setOfflineIcon();
	void setNormalIcon();
};

