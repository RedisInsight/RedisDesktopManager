#include <QTest>
#include <QApplication>

//tests
#include <iostream>
//#include "test_redisconnectionsmanager.h"
//#include "test_command.h"
//#include "test_response.h"
//#include "test_valueformatters.h"
//#include "test_valuetab.h"
//#include "test_connection.h"
#include "test_serveritem.h"
#include "test_databaseitem.h"

int main(int argc, char *argv[])
{
	QApplication app( argc, argv );   

//    qRegisterMetaType<RedisClient::AbstractProtocol::DatabaseList>("RedisClient::AbstractProtocol::DatabaseList");
//    qRegisterMetaType<RedisClient::Command>("Command");
//    qRegisterMetaType<RedisClient::Command>("RedisClient::Command");
//    qRegisterMetaType<RedisClient::Response>("Response");
//    qRegisterMetaType<RedisClient::Response>("RedisClient::Response");

	int allTestsResult = 
         QTest::qExec(new TestServerItem, argc, argv)
            + QTest::qExec(new TestDatabaseItem, argc, argv);
        //QTest::qExec(new TestConnection, argc, argv);// +
        //QTest::qExec(new TestValueTab, argc, argv) +
        //QTest::qExec(new TestCommand, argc, argv) +
//		QTest::qExec(new TestResponse, argc, argv) +
//		QTest::qExec(new TestRedisConnection, argc, argv) +
//		QTest::qExec(new TestRedisConnectionsManager, argc, argv) +
//		QTest::qExec(new TestValueFormatters, argc, argv);

    if (allTestsResult != 0 ) {

		#ifdef WIN32		
		std::cin.get();		
		#endif // WIN32

        return 1;
    }

	return 0;
}

