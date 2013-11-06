#pragma once

#include <QtCore>

class TestResponse;

class Response
{
	friend class TestResponse;

public:
	Response();
	Response(QString &);
	~Response(void);

	QVariant getValue();
	bool isValid();	

	void setSource(QString&);
	QString source();

	void clear();

	void appendToSource(QString&);
	void appendToSource(QByteArray&);

	static QString valueToString(QVariant&);

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

	int getSizeOfBulkReply(QString& reply, int endOfFirstLine = -1, int beginFrom = 0);	

	//validations
	bool isReplyValid(QString&);

	/** checks general validation rules **/
	bool isReplyGeneralyValid(QString& );
	bool isIntReplyValid(QString&);
	bool isBulkReplyValid(QString&);
	bool isMultiBulkReplyValid(QString&);	

	int getPosOfNextItem(QString &, int);
};

