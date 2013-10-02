#ifndef TEST_REDISCONNECTION_H
#define TEST_REDISCONNECTION_H

#include <QObject>
#include <QtCore>


class TestRedisConnection : public QObject
{
	Q_OBJECT

private slots:

	// unit tests
	void parseResponse();
	void parseResponse_data();
	
	void prepareCommand();	
	void isIntResponseValid();
	void isBulkReplyValid();
	void isMultiBulkReplyValid();

	// functional tests
	void connect();
	void execute();
	void getDatabases();		
};

#endif // TEST_REDISCONNECTION_H
