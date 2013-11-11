#pragma once

#include <QtCore>

class Command
{
public:
	Command();
	Command(const QString& cmdString, QObject * owner, int db = -1);

	bool isEmpty() const;

	bool hasDbIndex() const;
	int getDbIndex() const;

	/** @see http://redis.io/topics/protocol for more info **/	
	QString getFormattedString() const;

	QString getRawString() const;
	
	QObject * getOwner();

	static QString getFormatted(const QString&);

private:
	QObject * owner;
	QString commandString;
	int dbIndex;
};

