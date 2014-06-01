#ifndef ABSTRACTPROTOCOL_H
#define ABSTRACTPROTOCOL_H

#include <QObject>
#include "connection.h"
#include "abstracttransporter.h"

namespace RedisClient {

class AbstractProtocol : public QObject
{
    Q_OBJECT
public:
    AbstractProtocol(Connection * connection);    

    /**
     * List of databases with keys counters
     * @emit databesesLoaded
     **/
    typedef QHash <QString, int> DatabaseList;
    DatabaseList getDatabases();

    /**
     * Select db
     * see  http://redis.io/commands/select
     */
    bool selectDb(int index);

    /**
     * Execute info command
     */
    QStringList getInfo();

public slots:
    void auth();

signals:
    void authOk();
    void errorOccurred(const QString&);

protected:
    Connection * m_connection;
};

}
#endif // ABSTRACTPROTOCOL_H
