#ifndef ABSTRACTPROTOCOL_H
#define ABSTRACTPROTOCOL_H

#include "connection.h"
#include "abstracttransporter.h"

namespace RedisClient {

class AbstractProtocol
{
public:
    AbstractProtocol(Connection * connection);
    bool auth();

    /**
     * List of databases with keys counters
     * @emit databesesLoaded
     **/
    typedef QMap <QString, int> DatabaseList;
    DatabaseList getDatabases();

    /**
     * Select db
     * see  http://redis.io/commands/select
     */
    bool selectDb(int index);

protected:
    Connection * m_connection;
};

}
#endif // ABSTRACTPROTOCOL_H
