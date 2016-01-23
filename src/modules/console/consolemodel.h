#pragma once

#include <QObject>
#include <QSharedPointer>
#include "exception.h"

namespace RedisClient { class Connection; }

#pragma once

#include <QObject>

namespace Console {

    class Model : public QObject
    {
        Q_OBJECT
        ADD_EXCEPTION

    public:
        Model(QSharedPointer<RedisClient::Connection> connection);

        Q_INVOKABLE void init();
        QString getName();
        QSharedPointer<RedisClient::Connection> getConnection();

    public slots:
        void executeCommand(const QString&);

    signals:
        void changePrompt(const QString &text, bool showPrompt);
        void addOutput(const QString &text, QString resultType);

    private:
        QSharedPointer<RedisClient::Connection> m_connection;
        int m_current_db;
    };
}
