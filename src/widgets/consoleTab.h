#pragma once
#include <qconsole.h>

namespace RedisClient{
class ConnectionConfig;
}
class ConsoleConnectionWrapper;

class ConsoleTab: public QConsole
{
    Q_OBJECT
public:
    ConsoleTab(RedisClient::ConnectionConfig&);
    ~ConsoleTab(void);    
public slots:
    void setPrompt(const QString &, bool);    
private:    
    QSharedPointer<ConsoleConnectionWrapper> connection;
};

