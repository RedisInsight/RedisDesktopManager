#pragma once

#include <QObject>

class ConnectionBridge;

class KeyModel : public QObject
{
	Q_OBJECT

public:
	KeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex);
	~KeyModel(void);	

	QString getKeyTypeString();

	virtual void loadValue() = 0;

	QString getKeyName();

	void renameKey(const QString&);

//	void deleteKey();

signals:
	void valueLoaded(const QVariant&);	
	void keyRenamed();
	void keyRenameError(const QString&);

private slots:
	void loadedValue(const QVariant&);		
	void loadedRenameStatus(const QVariant&);

private:	
	QString keyName;
	int dbIndex;
	ConnectionBridge * db;
};

