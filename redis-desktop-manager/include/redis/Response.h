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

	int getLoadedItemsCount();

private:

	QString responseString;

	//type
	enum ResponseType 
	{
		Status, Error, Integer, Bulk, MultiBulk, Unknown			
	};

	//cache previous validation markers
	int lastValidPos;
	int itemsCount;

	ResponseType getResponseType(QString);	
	ResponseType getResponseType(const QChar);

	//parsers
	QVariant parseBulk(const QString& response);
	QStringList parseMultiBulk(const QString& response);
	QString getStringResponse(QString response);

	int getSizeOfBulkReply(const QString& reply, int endOfFirstLine = -1, int beginFrom = 0);	

	//validations
	bool isReplyValid(const QString&);

	/** checks general validation rules **/
	bool isReplyGeneralyValid(const QString& );
	bool isIntReplyValid(const QString&);
	bool isBulkReplyValid(const QString&);
	bool isMultiBulkReplyValid(const QString&);	

	int getPosOfNextItem(const QString &, int);
};

