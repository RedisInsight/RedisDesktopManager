#pragma once
#include "exception.h"
#include "common/tabviewmodel.h"

namespace ServerStats {

    class Model : public TabModel
    {
        Q_OBJECT
        ADD_EXCEPTION

        Q_PROPERTY(QVariantMap serverInfo READ serverInfo NOTIFY serverInfoChanged)

    public:
        Model(QSharedPointer<RedisClient::Connection> connection);

        Q_INVOKABLE void init() override;

        QString getName() const override;

        QVariantMap serverInfo();

    signals:
        void error(const QString& error);

        void initialized();

        void serverInfoChanged();

    private:
        QTimer m_updateTimer;
        QVariantMap m_serverInfo;
    };
}
