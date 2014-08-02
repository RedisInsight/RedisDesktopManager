#include "consoletab.h"

#include <QScrollBar>
#include <QTextBlock>
#include <QBoxLayout>

#include <qconsole.h>
#include "core/connectionconfig.h"
#include "core/protocols/consoleconnectionwrapper.h"

using namespace Console;

ConsoleTab::ConsoleTab(QSharedPointer<Operations> operations)
    : m_consoleWidget(new QConsole(nullptr, "<span style='color: orange;'>"
                                   "List of unsupported commands: PTTL, DUMP, RESTORE, AUTH, QUIT, MONITOR"
                                   "</span> <br /> Connecting ...")),
      m_consoleOperations(operations)
{        
    // init console widget
    QPalette p = palette();
    p.setColor(QPalette::Base, QColor(57, 57, 57));
    p.setColor(QPalette::Text, QColor(238, 238, 238));
    m_consoleWidget->setPalette(p);
    m_consoleWidget->setCmdColor(Qt::yellow);        
    m_consoleWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    // init internal layout
    m_consoleWidget->setParent(this);
    QBoxLayout * layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    layout->setMargin(0);
    layout->addWidget(m_consoleWidget.data());
    setLayout(layout);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    //connect widget & console operations model
    connect(m_consoleWidget.data(), &QConsole::execCommand,
            m_consoleOperations.data(), &Operations::executeCommand);
    connect(m_consoleOperations.data(), &Operations::changePrompt,
            this, &ConsoleTab::setPrompt);
    connect(m_consoleOperations.data(), &Operations::addOutput,
            m_consoleWidget.data(), &QConsole::printCommandExecutionResults);

    connect(&m_initTimer, &QTimer::timeout,
            this, [this]() { m_consoleOperations->init(); });

    m_initTimer.setSingleShot(true);
    m_initTimer.start(0);
}

void ConsoleTab::close()
{

}

bool ConsoleTab::shouldBeReplaced()
{
    return false;
}

QString ConsoleTab::getTitle()
{
    return m_consoleOperations->getConsoleName();
}

QIcon ConsoleTab::getIcon()
{
    return QIcon(":/images/terminal.png");
}

void ConsoleTab::setPrompt(const QString & str, bool display)
{
    m_consoleWidget->setPrompt(str, display);
}
