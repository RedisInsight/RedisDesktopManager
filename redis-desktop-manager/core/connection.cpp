#include "connection.h"
#include "defaulttransporter.h"
#include "sshtransporter.h"

RedisClient::Connection::Connection(const Config &c, bool autoConnect)
{
    if (c.useSshTunnel()) {
        transporter = new QSharedPointer(new SshTransporter());
    } else {
       transporter = new QSharedPointer(new DefaultTransporter());
    }

    protocol = new QSharedPointer(new )


    if (autoConnect)
        connect();
}
