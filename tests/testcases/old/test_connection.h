#ifndef TEST_CONNECTION_H
#define TEST_CONNECTION_H

#include <QObject>
#include <QtCore>
#include "connectionconfig.h"

class TestConnection : public QObject
{
    Q_OBJECT

private slots:
    void init();

    /*
     * connect() & disconnect() tests
     */
    void connectAndDisconnect();
    void connectToHostAndRunCommand();
    void connectWithAuth();
    void connectWithSshTunnelPass();
    void connectWithSshTunnelKey();

    /*
     * default protocol tests
     */
    void selectDatabase();
    void selectDatabase_data();

    /*
     * dirty tests for runCommand()
     */
    void runEmptyCommand();
    void runCommandWithoutConnection();
    void runCommandAndDelete();

private:
    RedisClient::ConnectionConfig config;
    void setSshSettings(RedisClient::ConnectionConfig&, bool);
};

#endif // TEST_CONNECTION_H

