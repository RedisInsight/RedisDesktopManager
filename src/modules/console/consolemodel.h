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

        void init();
        QString getName();
        QSharedPointer<RedisClient::Connection> getConnection();

    public slots:
        void executeCommand(const QString&);

    signals:
        void changePrompt(const QString &, bool);
        void addOutput(const QString&, QString resultType);

    private:
        QSharedPointer<RedisClient::Connection> m_connection;
        int m_current_db;
    };
}
