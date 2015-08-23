#pragma once

#include "command.h"
#include "commandexecutor.h"
#include "connection.h"
#include "connectionconfig.h"
#include "response.h"
#include <QObject>

inline void initRedisClient()
{
    qRegisterMetaType<RedisClient::Command>("Command");
    qRegisterMetaType<RedisClient::Command>("RedisClient::Command");
    qRegisterMetaType<RedisClient::Response>("Response");
    qRegisterMetaType<RedisClient::Response>("RedisClient::Response");
}
