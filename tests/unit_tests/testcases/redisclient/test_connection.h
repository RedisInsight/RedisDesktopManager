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
    void connectWithSshTunnelPass();
    void connectWithSshTunnelKey();


    void selectDatabase();
    void selectDatabase_data();

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

private:
    RedisClient::ConnectionConfig config;
    void setSshSettings(RedisClient::ConnectionConfig&, bool);
};

#endif // TEST_CONNECTION_H

