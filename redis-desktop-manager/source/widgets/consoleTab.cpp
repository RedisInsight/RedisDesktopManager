#include "consoleTab.h"

#include <QScrollBar>
#include <QTextBlock>

#include "RedisConnectionConfig.h"
#include "ConsoleConnectionWrapper.h"

consoleTab::consoleTab(RedisConnectionConfig& config)
    : QConsole(nullptr, "<span style='color: orange;'>"
                        "List of unsupported commands: PTTL, DUMP, RESTORE, AUTH, QUIT, MONITOR"
                        "</span> <br /> Connecting ...")
{    
    setObjectName("consoleTab");

     QPalette p = palette();
     p.setColor(QPalette::Base, QColor(57, 57, 57));
     p.setColor(QPalette::Text, QColor(238, 238, 238));
     setPalette(p);
    
    setCmdColor(Qt::yellow);

    connection = new ConsoleConnectionWrapper(config);
    connection->moveToThread(&connectionThread);

    connect(&connectionThread, &QThread::finished, connection, &QObject::deleteLater);
    connect(this, SIGNAL(execCommand(const QString &)), connection, SLOT(executeCommand(const QString&)));    
    connect(connection, SIGNAL(changePrompt(const QString &, bool)), this, SLOT(setPrompt(const QString &, bool)));
    connect(connection, SIGNAL(addOutput(const QString&)), this, SLOT(printCommandExecutionResults(const QString&)));
    connect(&connectionThread, SIGNAL(started()), connection, SLOT(init()));
    
    connectionThread.start();
}

consoleTab::~consoleTab(void)
{
    connectionThread.quit();
    connectionThread.wait();
}

void consoleTab::setPrompt(const QString & str, bool display)
{
    QConsole::setPrompt(str, display);
}