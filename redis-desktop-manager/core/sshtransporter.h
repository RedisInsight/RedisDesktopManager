#ifndef SSHTRANSPORTER_H
#define SSHTRANSPORTER_H

#include "abstracttransporter.h"

namespace RedisClient {

class SshTransporter : public AbstractTransporter
{
public:
    SshTransporter(Connection *);
};

}

#endif // SSHTRANSPORTER_H
