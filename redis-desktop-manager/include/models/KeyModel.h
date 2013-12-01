#pragma once

#include <QStandardItemModel>
#include "ConnectionBridge.h"
#include "Command.h"

class KeyModel : public QStandardItemModel
{
	Q_OBJECT

public:
	KeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex);	

	virtual void loadValue() = 0;

	QString getKeyName();

	void renameKey(const QString&);

	const static int DEFAULT_TYPE = 1;

	inline virtual int type()
	{
		return DEFAULT_TYPE;
	}

signals:	
	void valueLoaded();
	void keyRenamed();
	void keyRenameError(const QString&);

protected slots:
	void loadedValue(const QVariant&);		
	void loadedRenameStatus(const QVariant&);

protected:	
	QString keyName;
	int dbIndex;
	ConnectionBridge * db;
};

