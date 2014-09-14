#pragma once

#include <QObject>
#include "abstractkey.h"

class StringKeyModel : public KeyModel
{
    Q_OBJECT

public:
    StringKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl);

    QStringList getColumnNames() override;
    QHash<int, QByteArray> getRoles() override;
    QString getData(int rowIndex, int dataRole) override;




};

