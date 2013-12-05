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
	Command(const QStringList& cmd, QObject * owner, const QString& invokeMethod, int db = -1);
	Command(const QStringList& cmd, QObject * owner, int db = -1);

	bool isEmpty() const;

	bool hasDbIndex() const;
	int getDbIndex() const;

	/** @see http://redis.io/topics/protocol for more info **/	
	QString getFormattedString() const;

	QString getRawString() const;
	
	QObject * getOwner();
	void setOwner(QObject *);

	static QString getFormatted(const QStringList&);

	static QString getFormatted(const QString&);

	bool hasCallback();

	QString getCallbackName();
	void setCallBackName(const QString &);

private:
	QObject * owner;
	QString callBackMethod;
	QStringList commandWithArguments;
	int dbIndex;

	QStringList splitCommandString(const QString &);
};

