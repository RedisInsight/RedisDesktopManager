#pragma once
#include <QObject>
#include <qredisclient/connection.h>

class TabModel : public QObject
{
    Q_OBJECT

    public:
        TabModel(QSharedPointer<RedisClient::Connection> connection);

        virtual ~TabModel() {}

        virtual QString getName() const = 0;

        Q_INVOKABLE virtual void init() = 0;

        virtual QSharedPointer<RedisClient::Connection> getConnection() const;

    protected:
        QSharedPointer<RedisClient::Connection> m_connection;
};
