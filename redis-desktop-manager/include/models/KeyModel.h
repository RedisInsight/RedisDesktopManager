#pragma once

#include <QStandardItemModel>
#include "ConnectionBridge.h"
#include "Command.h"

class KeyModel : public QStandardItemModel
{
	Q_OBJECT

public:
	KeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex);	
	virtual ~KeyModel();	

	virtual void loadValue() = 0;

	QString getKeyName();

	void renameKey(const QString&);

	void deleteKey();

	const static int KEY_MODEL_TYPE = 1;

	inline virtual int getKeyModelType()
	{
		return KEY_MODEL_TYPE;
	}

signals:	
	void valueLoaded();
	void keyRenamed();
	void keyRenameError(const QString&);
	void keyDeleted();
	void keyDeleteError(const QString&);

protected slots:
	void loadedValue(Response);		
	void loadedRenameStatus(Response);
	void loadedDeleteStatus(Response);

protected:	
	QString keyName;
	int dbIndex;
	ConnectionBridge * db;
	bool eventsBlocked;

	virtual void initModel(const QVariant &) = 0;
};


