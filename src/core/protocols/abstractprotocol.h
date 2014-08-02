#ifndef ABSTRACTPROTOCOL_H
#define ABSTRACTPROTOCOL_H

#include <QObject>
#include <QSharedPointer>
#include "core/connection.h"
#include "core/transporters/abstracttransporter.h"
#include "connections-tree/operations.h"

namespace RedisClient {

class AbstractProtocol : public QObject, public ConnectionsTree::Operations
{
    Q_OBJECT
public:
    AbstractProtocol(Connection* connection);

    /**
     * Select db
     * see  http://redis.io/commands/select
     */
    bool selectDb(int index);

    /**
     * Execute info command
     */
    QStringList getInfo();


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

public slots:
    void auth();

signals:
    void authOk();
    void errorOccurred(const QString&);

protected:
    Connection* m_connection;
};

}
#endif // ABSTRACTPROTOCOL_H
