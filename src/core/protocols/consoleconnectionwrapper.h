#pragma once

#include <QObject>
#include "core/connectionconfig.h"
#include "console/operations.h"

namespace RedisClient {
class Connection;
}

class ConsoleConnectionWrapper : public Console::Operations
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

