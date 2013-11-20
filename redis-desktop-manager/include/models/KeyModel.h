#pragma once
#pragma once

#include <QObject>
#include "ConnectionBridge.h"

class KeyModel : public QObject
{
	Q_OBJECT

public:
	KeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex);
	~KeyModel(void);

	enum Type {String, Hash, List, Set, ZSet, None, Empty};

	void getKeyType();

	void getValue();

	QString getKeyName();

signals:
	void valueLoaded(const QVariant&, QObject *);
	void keyTypeLoaded(KeyModel::Type);

private slots:
	void loadedValue(const QVariant&, QObject *);
	void loadedType(const QVariant&, QObject *);

private:
	Type keyType;
	QString keyName;
	int dbIndex;
	ConnectionBridge * db;
};

