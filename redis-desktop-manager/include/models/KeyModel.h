#pragma once

#include <QObject>
#include <QHash>

class RedisKeyItem;
class ConnectionBridge;

class KeyModel : public QObject
{
	Q_OBJECT

public:
	KeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex, RedisKeyItem * key);
	~KeyModel(void);

	enum Type {String = 0, Hash = 1, List = 2, Set = 3, ZSet = 4, None = 5, Empty = 5};	

	void getKeyType();

	QString getKeyTypeString();

	void getValue();

	QString getKeyName();

	static QString TypeToString(Type t);

	void renameKey(const QString&);

//	void deleteKey();

signals:
	void valueLoaded(const QVariant&, QObject *);
	void keyTypeLoaded(KeyModel::Type);
	void keyRenamed();

private slots:
	void loadedValue(const QVariant&);
	void loadedType(const QVariant&);	
	void loadedRenameStatus(const QVariant&);

private:
	RedisKeyItem * key;
	Type keyType;
	QString keyTypeString;
	QString keyName;
	int dbIndex;
	ConnectionBridge * db;
};

