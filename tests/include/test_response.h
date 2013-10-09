#pragma once

#include <QObject>

class TestResponse: public QObject
{
	Q_OBJECT

private slots:
	//parsing tests
	void parseResponse();
	void parseResponse_data();

	//validation tests
	void isIntResponseValid();
	void isBulkReplyValid();
	void isMultiBulkReplyValid();
};

