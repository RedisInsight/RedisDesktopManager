#pragma once


#include "qconsole.h"
#include "BaseTab.h"
#include <QThread>

class RedisConnectionConfig;
class ConsoleConnectionWrapper;

class ConsoleTab: public QConsole
{
    Q_OBJECT

public:
    ConsoleTab(RedisConnectionConfig&);
    ~ConsoleTab(void);    

public slots:
    void setPrompt(const QString &, bool);    

private:    
    ConsoleConnectionWrapper * connection;
    QThread connectionThread;    
};

