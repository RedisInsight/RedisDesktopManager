#pragma once

#include <QObject>
#include "KeyModel.h"

class StringKeyModel : public KeyModel
{
	Q_OBJECT

public:
	StringKeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex);

	void loadValue();

signals:
	void valueLoaded(const QVariant&);
	void keyRenamed();
	void keyRenameError(const QString&);

};

