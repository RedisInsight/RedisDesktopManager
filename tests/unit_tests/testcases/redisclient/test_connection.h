#ifndef TEST_CONNECTION_H
#define TEST_CONNECTION_H

#include <QObject>
#include <QtCore>
#include "basetestcase.h"
#include "redisclient/connectionconfig.h"

class TestConnection : public BaseTestCase
{
    Q_OBJECT

private slots:
    void init();

#ifdef INTEGRATION_TESTS

    /*
     * connect() & disconnect() tests
     */
    void connectAndDisconnect();
    void connectToHostAndRunCommand();
    void connectWithAuth();
    void connectWithInvalidAuth();
    void connectWithInvalidConfig();

    void selectDatabase();
    void selectDatabase_data();

    void testScanCommand();
    void testRetriveCollection();

    /*
     * dirty tests for runCommand()
     */
    void runEmptyCommand();
    void runCommandWithoutConnection();
    void runCommandAndDelete();
#endif

    /*
     * Dummy transporter test
     */
    void testWithDummyTransporter();

    void testParseServerInfo();
    void testConfig();

private:
    RedisClient::ConnectionConfig config;
};

#endif // TEST_CONNECTION_H

