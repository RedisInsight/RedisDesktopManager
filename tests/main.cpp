#include <QTest>
#include <QApplication>

//tests
#include <iostream>
#include "test_redisconnection.h"
#include "test_redisconnectionsmanager.h"

int main(int argc, char *argv[])
{
	QApplication app( argc, argv );

	QTest::qExec(new TestRedisConnection, argc, argv);
	QTest::qExec(new TestRedisConnectionsManager, argc, argv);

	std::cin.get();
	std::cin.get();

	return 0;
}

