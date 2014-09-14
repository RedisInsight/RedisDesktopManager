#pragma once

#include <QSharedPointer>
#include <QString>
#include "modules/value-editor/model.h"
#include "modules/redisclient/connection.h"

class KeyModel : public ValueEditor::Model
{
    Q_OBJECT
public:
    KeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl);

    QString getKeyName() override;
    int getTTL() override;

    bool isPartialLoadingSupported() override;

    void setKeyName(const QString&) override;
    void setTTL(int) override;
    void removeKey() override;

protected:
    QSharedPointer<RedisClient::Connection> m_connection;
    QString m_keyFullPath;
    int m_dbIndex;
    int m_ttl;
    bool m_isKeyRemoved;
};


