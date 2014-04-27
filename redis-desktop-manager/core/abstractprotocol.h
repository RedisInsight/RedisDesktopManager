#ifndef ABSTRACTPROTOCOL_H
#define ABSTRACTPROTOCOL_H

#include "connection.h"
#include "abstracttransporter.h"

namespace RedisClient {


class AbstractProtocol
{
private:
    AbstractProtocol(Connection * connection, AbstractTransporter * transporter);
public:
    QString version();
    void getDatabases();

    /**
     * Select db
     * see  http://redis.io/commands/select
     */
    bool selectDb(int index);

    /**
     * List of databases with keys counters
     * @emit databesesLoaded
     **/
    typedef QMap <QString, int> DatabaseList;
};

}
#endif // ABSTRACTPROTOCOL_H
