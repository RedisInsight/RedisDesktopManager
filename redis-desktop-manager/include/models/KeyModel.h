#pragma once

#include <QObject>

class ConnectionBridge;

class KeyModel : public QObject
{
	Q_OBJECT

public:
	KeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex);
	~KeyModel(void);

	enum Type {String = 0, Hash = 1, List = 2, Set = 3, ZSet = 4, None = 5, Empty = 5};	

	QString getKeyTypeString();

	void getValue();

	QString getKeyName();

	void renameKey(const QString&);

//	void deleteKey();

signals:
	void valueLoaded(const QVariant&);
	void keyTypeLoaded(KeyModel::Type);
	void keyRenamed();
	void keyRenameError(const QString&);

private slots:
	void loadedValue(const QVariant&);
	void loadedType(const QVariant&);	
	void loadedRenameStatus(const QVariant&);

private:
	Type keyType;
	QString keyTypeString;
	QString keyName;
	int dbIndex;
	ConnectionBridge * db;
};

