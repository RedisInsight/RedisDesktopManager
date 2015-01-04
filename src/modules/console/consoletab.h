#pragma once
#include <QTimer>
#include "basetab.h"
#include "operations.h"

class QConsole;

namespace Console {

class ConsoleTab: public BaseTab
{
    Q_OBJECT
public:
    ConsoleTab(QSharedPointer<Operations>);

    void close() override;
    virtual QString getTitle() override;
    virtual QIcon getIcon() override;

private slots:
    void setPrompt(const QString &, bool);

private:    
    QSharedPointer<QConsole> m_consoleWidget;
    QSharedPointer<Operations> m_consoleOperations;
    QTimer m_initTimer;
};
}

