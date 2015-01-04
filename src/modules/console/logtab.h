#pragma once
#include "basetab.h"
#include <QWidget>
#include <QDateTime>
#include <easylogging++.h>

class QConsole;

namespace Console {

class LogTab : public BaseTab
{
    Q_OBJECT
public:
    LogTab();
    ~LogTab();

    void close() override;
    virtual QString getTitle() override;
    virtual QIcon getIcon() override;

    void print(QString log);

private:
    QSharedPointer<QConsole> m_consoleWidget;
};

class LogHandler : public el::LogDispatchCallback {
public:

    LogHandler()
        : el::LogDispatchCallback(), m_tab(nullptr)
    {}

    void setOutputTab(QSharedPointer<LogTab> tab)
    {
        m_tab = tab;
    }

    void handle(const el::LogDispatchData* data) {
        if (m_tab)
            m_tab->print(QString("%1 : %2 (%3)")
                         .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                         .arg(QString::fromStdString(data->logMessage()->message()))
                         .arg(QString::fromStdString(data->logMessage()->func()))
                         );
    }
private:
    QSharedPointer<LogTab> m_tab;
};

}
