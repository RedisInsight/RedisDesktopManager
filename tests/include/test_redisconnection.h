#ifndef TEST_REDISCONNECTION_H
#define TEST_REDISCONNECTION_H

#include <QObject>
#include <QtCore>


class TestRedisConnection : public QObject
{
	Q_OBJECT

private slots:

	// functional tests
	void connect();
	void execute();
	void runCommand();
};

#endif // TEST_REDISCONNECTION_H
