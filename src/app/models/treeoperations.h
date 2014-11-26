#pragma once

#include <QSharedPointer>
#include <QObject>
#include "modules/redisclient/connection.h"
#include "modules/connections-tree/operations.h"

class ConsoleTabs;

class TreeOperations : public QObject, public ConnectionsTree::Operations
{
    Q_OBJECT
public:
    TreeOperations(QSharedPointer<RedisClient::Connection> connection,
                   ConsoleTabs& tabs);

    void getDatabases(std::function<void(DatabaseList)>) override;

    void getDatabaseKeys(uint dbIndex, std::function<void(const RawKeysList&)>) override;

    void disconnect() override;

    QString getNamespaceSeparator() override;    

    virtual void openKeyTab(const QString& fullPath, int dbIndex,
                            bool openInNewTab = false) override;

    virtual void openConsoleTab() override;

signals:
    void openValueTab(QSharedPointer<RedisClient::Connection> connection,
                      const QString& fullPath, int dbIndex, bool inNewTab);

private:
     QSharedPointer<RedisClient::Connection> m_connection;
     ConsoleTabs& m_consoleTabs;
};
