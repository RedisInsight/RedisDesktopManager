#pragma once

#include <QObject>
#include <QSharedPointer>
#include "modules/console/operations.h"

namespace RedisClient { class Connection; }

class ConsoleModel : public Console::Operations
{
    Q_OBJECT
public:
    ConsoleModel(QSharedPointer<RedisClient::Connection>);

public:
    void init();
    QString getConsoleName() override;
    void executeCommand(const QString&);

private:
    QSharedPointer<RedisClient::Connection> m_connection;
    int m_current_db;
};
