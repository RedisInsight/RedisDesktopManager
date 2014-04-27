#ifndef TEST_CONNECTION_H
#define TEST_CONNECTION_H


#include <QObject>
#include <QtCore>
#include "connection.h"
#include "commandexecutor.h"

class TestConnection : public QObject
{
    Q_OBJECT

private slots:

    void connectToHostAndRunCommand()
    {
        using namespace RedisClient;

        Config config;
        Connection connection(config, true);

        if (!connection.isConnected()) {
            qDebug() << "Error occured: " << connection.getLastError();
            return;
        }

        Command cmd("ping");

        //sync execution
        Response result = CommandExecutor::execute(connection, cmd);


    }

};


#endif // TEST_CONNECTION_H
