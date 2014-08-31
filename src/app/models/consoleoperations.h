#pragma once

#include <QObject>
#include "redisclient/connectionconfig.h"
#include "console/operations.h"

namespace RedisClient {
class Connection;
}

class ConsoleModel : public Console::Operations
{
    Q_OBJECT
public:
    ConsoleConnectionWrapper(RedisClient::Connection*);

    public:
        void init();
        QString getConsoleName() override;
        void executeCommand(const QString&);

private:
    RedisClient::Connection* m_connection;

};

