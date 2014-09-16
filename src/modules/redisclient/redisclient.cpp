#include "redisclient.h"

void initRedisClient()
{
    qRegisterMetaType<RedisClient::Command>("Command");
    qRegisterMetaType<RedisClient::Command>("RedisClient::Command");
    qRegisterMetaType<RedisClient::Response>("Response");
    qRegisterMetaType<RedisClient::Response>("RedisClient::Response");
}
