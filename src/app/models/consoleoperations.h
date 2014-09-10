#pragma once

#include <QObject>
#include "modules/redisclient/connectionconfig.h"
#include "modules/console/operations.h"

namespace RedisClient {
class Connection;
}

class ConsoleModel : public Console::Operations
{
    Q_OBJECT
public:
    ConsoleModel(RedisClient::Connection*);

    public:
        void init();
        QString getConsoleName() override;
        void executeCommand(const QString&);

private:
    RedisClient::Connection* m_connection;

};

