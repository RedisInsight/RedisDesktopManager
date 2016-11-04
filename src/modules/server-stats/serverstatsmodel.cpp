#include "serverstatsmodel.h"


ServerStats::Model::Model(QSharedPointer<RedisClient::Connection> connection)
    : TabModel(connection)
{
    m_updateTimer.setInterval(5000);
    m_updateTimer.setSingleShot(false);

    QObject::connect(&m_updateTimer, &QTimer::timeout, this, [this]{

        QList<QByteArray> rawCmd {"INFO", "all"};
        m_connection->command(rawCmd, this, [this](RedisClient::Response r, QString err) {

            if (!err.isEmpty()) {
                emit error(QObject::tr("Cannot update server info tab. Error: %0").arg(err));
                return;
            }

            m_serverInfo = RedisClient::ServerInfo::fromString(r.toRawString()).parsed.toVariantMap();
            emit serverInfoChanged();
        });

    });
}

void ServerStats::Model::init()
{
    try {
        if (!m_connection->connect())
        {
            emit error(QObject::tr("Connection error. Check network connection"));
            return;
        }

        m_updateTimer.start();

    } catch (RedisClient::Connection::Exception&) {
        emit error(QObject::tr("Invalid Connection. Check connection settings."));
        return;
    }

    emit initialized();
}

QString ServerStats::Model::getName() const
{
    return QString(QObject::tr("Server %0")).arg(m_connection->getConfig().name());
}

QVariantMap ServerStats::Model::serverInfo()
{
    return m_serverInfo;
}
