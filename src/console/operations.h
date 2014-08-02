#pragma once

#include <QObject>
#include <qconsole.h>

namespace Console {

    class Operations : public QObject
    {
        Q_OBJECT

    public:
        virtual void init() = 0;
        virtual QString getConsoleName() = 0;

    public slots:
        virtual void executeCommand(const QString&) = 0;

    signals:
        void changePrompt(const QString &, bool);
        void addOutput(const QString&, QConsole::ResultType);
    };
}
