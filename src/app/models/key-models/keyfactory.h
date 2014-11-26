#pragma once

#include "modules/value-editor/abstractkeyfactory.h"

class KeyFactory : public QObject, public ValueEditor::AbstractKeyFactory
{
    Q_OBJECT
public:
    KeyFactory();

    void loadKey(QSharedPointer<RedisClient::Connection> connection,
                 QString keyFullPath, int dbIndex,
                 std::function<void(QSharedPointer<ValueEditor::Model>)> callback) override;
};


