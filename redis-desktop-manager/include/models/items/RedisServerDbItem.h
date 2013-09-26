#pragma once

#include <QStandardItem>
class RedisServerItem;
class RedisKeyItem;

class RedisServerDbItem : public QStandardItem
{
	friend class RedisKeyItem;
public:
	RedisServerDbItem(QString name, int keysCount, RedisServerItem * parent);	

	void loadKeys();	

	int virtual type() const;

	const static int TYPE = 2100;

	void setCurrent();

	int getDbIndex() const;

    bool operator<(const QStandardItem & other) const;

private:
	RedisServerItem * server;
	bool isKeysLoaded;
	int dbIndex;

	void renderNamaspacedKey(QStandardItem * currItem, QString notProcessedKeyPart, QString fullKey);

	void setNormalIcon();
	void setBusyIcon();
};

