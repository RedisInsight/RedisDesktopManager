#pragma once

#include <QObject>
#include <connectionconfig.h>

namespace RedisClient {
class Connection;
}

class ConsoleConnectionWrapper : public QObject
{
    Q_OBJECT

public:
    ConsoleConnectionWrapper(RedisClient::ConnectionConfig &);

    public slots:
        void init();
        void executeCommand(const QString &);

    signals:
        void changePrompt(const QString &, bool);
        void addOutput(const QString &);

private:
    QSharedPointer<RedisClient::Connection> connection;
    RedisClient::ConnectionConfig config;
    bool connectionValid;    
};

