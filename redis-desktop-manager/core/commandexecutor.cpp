#include "commandexecutor.h"


Response RedisClient::CommandExecutor::execute(RedisClient::Connection *, const Command &cmd)
{
    return Response();
}
