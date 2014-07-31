#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <QObject>
#include <qconsole.h>

namespace Console {

    class Operations : public QObject
    {
        Q_OBJECT

    public slots:
        virtual void executeCommand(const QString&) = 0;

    signals:
        void changePrompt(const QString &, bool);
        void addOutput(const QString&, QConsole::ResultType);
    };
}

#endif // OPERATIONS_H
