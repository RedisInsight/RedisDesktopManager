#include "logtab.h"
#include <QScrollBar>
#include <QTextBlock>
#include <QBoxLayout>

#include <qconsole.h>

using namespace Console;

LogTab::LogTab()
  : m_consoleWidget(new QConsole(nullptr, ""))
{
    // init console widget
    QPalette p = palette();
    p.setColor(QPalette::Base, QColor(255, 255, 255));
    p.setColor(QPalette::Text, QColor(0, 0, 0));
    m_consoleWidget->setPalette(p);
    m_consoleWidget->setCmdColor(Qt::yellow);
    m_consoleWidget->setOutColor(QColor(140, 140, 140));
    m_consoleWidget->setReadOnly(true);
    m_consoleWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    // init internal layout
    m_consoleWidget->setParent(this);
    QBoxLayout * layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    layout->setMargin(0);
    layout->addWidget(m_consoleWidget.data());
    setLayout(layout);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
}

LogTab::~LogTab()
{

}

void LogTab::close()
{

}

QString LogTab::getTitle()
{
    return QString("System log");
}

QIcon LogTab::getIcon()
{
    return QIcon(":/images/terminal.png");
}

void LogTab::print(QString log)
{
    m_consoleWidget->printCommandExecutionResults(
                log, QConsole::ResultType::Partial);
}

