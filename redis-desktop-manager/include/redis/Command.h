#pragma once

#include <QtCore>

#ifndef CALLBACK_MACRO
	#define CALLMETHOD(x) x  //just syntax sugar for clarify purpose of string 
	#define CALLBACK_MACRO
#endif

class Command
{
public:
	Command();
	Command(const QString& cmdString, QObject * owner, int db = -1);
	Command(const QString& cmdString, QObject * owner, const QString& invokeMethod, int db = -1);

	bool isEmpty() const;

	bool hasDbIndex() const;
	int getDbIndex() const;

	/** @see http://redis.io/topics/protocol for more info **/	
	QString getFormattedString() const;

	QString getRawString() const;
	
	QObject * getOwner();

	static QString getFormatted(const QString&);

	bool hasCallback();

	QString getCallbackName();

private:
	QObject * owner;
	QString callBackMethod;
	QString commandString;
	int dbIndex;
};

