#pragma once

#include "modules/value-editor/abstractkeyfactory.h"
#include "modules/redisclient/redisclient.h"

class KeyFactory : public QObject, public ValueEditor::AbstractKeyFactory
{
    Q_OBJECT
public:
    KeyFactory(QSharedPointer<RedisClient::Connection> connection);

    void loadKey(QString keyFullPath, int dbIndex, std::function<void(QSharedPointer<ValueEditor::Model>)> callback) override;

protected:
    QSharedPointer<RedisClient::Connection> m_connection;

};


