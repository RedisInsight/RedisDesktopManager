#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <easylogging++.h>

class LogHandler : public QObject, public el::LogDispatchCallback
{
    Q_OBJECT

public:
    LogHandler() : QObject::QObject(), el::LogDispatchCallback() {}

    void handle(const el::LogDispatchData* data) {

        QString msg = QString("%1 : %2")
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                .arg(QString::fromStdString(data->logMessage()->message()));

        if (receivers(SIGNAL(event(QString))) == 0)
            m_messages.push_back(msg);
        else
            emit event(msg);
    }

public slots:
    void getMessages()
    {
        while (true) {
            if (m_messages.isEmpty())
                break;

            emit event(m_messages.takeFirst());
        }
    }

signals:
    void event(QString msg);

private:
    QStringList m_messages;
};

