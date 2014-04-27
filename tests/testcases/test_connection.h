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

        RedisConnectionConfig config("127.0.0.1");
        Connection connection(config, true);

        Command cmd("ping");

        connection.runCommand(cmd);

        //sync execution
        //Response result = CommandExecutor::execute(connection, cmd);

    }

};


#endif // TEST_CONNECTION_H
