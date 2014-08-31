#pragma once

#include <QObject>
#include <QSharedPointer>
#include "redisclient/connection.h"
#include "connections-tree/operations.h"

class TreeOperations : public ConnectionsTree::Operations
{
public:
    TreeOperations(QSharedPointer<RedisClient::Connection> connection);

    /**
     * @brief getDatabases
     */
    void getDatabases(std::function<void(DatabaseList)>) override;

    /**
     * @brief getDatabaseKeys
     * @param dbIndex
     */
    void getDatabaseKeys(uint dbIndex, std::function<void(const RawKeysList&)>) override;

    QSharedPointer<Console::Operations> getConsoleOperations() override;

    void disconnect() override;

    QString getNamespaceSeparator() override;
};
