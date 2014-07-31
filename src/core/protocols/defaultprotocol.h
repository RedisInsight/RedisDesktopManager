#ifndef DEFAULTPROTOCOL_H
#define DEFAULTPROTOCOL_H

#include "abstractprotocol.h"

namespace RedisClient {

class DefaultProtocol : public AbstractProtocol
{
public:
    DefaultProtocol(Connection * connection);
};
}

#endif // DEFAULTPROTOCOL_H
