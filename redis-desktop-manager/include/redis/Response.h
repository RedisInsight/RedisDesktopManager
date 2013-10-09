#pragma once

#include <QtCore>

class TestResponse;

class Response
{
	friend class TestResponse;

public:
	Response();
	~Response(void);

	QVariant getValue();
	bool isValid();	

	void setSource(QString&);
	QString source();
	void appendToSource(QString&);
	void appendToSource(QByteArray&);

private:

	QString responseString;

	//type
	enum ResponseType 
	{
		Status, Error, Integer, Bulk, MultiBulk, Unknown			
	};

	ResponseType getResponseType(QString);	
	ResponseType getResponseType(const QChar);

	//parsers
	QVariant parseBulk(QString response);
	QStringList parseMultiBulk(QString response);
	QString getStringResponse(QString response);
	int getSizeOfBulkReply(QString mb);	

	//validations
	bool isIntReplyValid(QString r);
	bool isBulkReplyValid(QString r);
	bool isMultiBulkReplyValid(QString r);	
};

