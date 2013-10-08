#include <QTest>
#include <QApplication>

//tests
#include <iostream>
#include "test_redisconnection.h"
#include "test_redisconnectionsmanager.h"

int main(int argc, char *argv[])
{
	QApplication app( argc, argv );

    if (QTest::qExec(new TestRedisConnection, argc, argv) != 0 ||
        QTest::qExec(new TestRedisConnectionsManager, argc, argv) != 0) {
        return 1;
    }

	return 0;
}

