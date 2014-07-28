#include "consoleTab.h"

#include <QScrollBar>
#include <QTextBlock>

#include "core/connectionconfig.h"
#include "core/consoleconnectionwrapper.h"

ConsoleTab::ConsoleTab(RedisClient::ConnectionConfig& config)
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

    connection = QSharedPointer<ConsoleConnectionWrapper>(new ConsoleConnectionWrapper(config));

    connect(this, SIGNAL(execCommand(const QString &)), connection.data(), SLOT(executeCommand(const QString&)));
    connect(connection.data(), SIGNAL(changePrompt(const QString &, bool)), this, SLOT(setPrompt(const QString &, bool)));
    connect(connection.data(), SIGNAL(addOutput(const QString&)), this, SLOT(printCommandExecutionResults(const QString&)));

    connection->init();
}

ConsoleTab::~ConsoleTab(void)
{    
}

void ConsoleTab::setPrompt(const QString & str, bool display)
{
    QConsole::setPrompt(str, display);
}
