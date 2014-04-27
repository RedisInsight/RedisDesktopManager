#ifndef COMMANDEXECUTOR_H
#define COMMANDEXECUTOR_H

#include "connection.h"
#include "Command.h"
#include "Response.h"

namespace RedisClient {

class CommandExecutor
{
public:
    static Response execute(Connection *, const Command& cmd);
};

}
#endif // COMMANDEXECUTOR_H
