#pragma once
#include <QObject>

#include "basetestcase.h"
#include "models/connectionconf.h"

template<typename T>
static void fakeDeleter(T*) {}

class RESPBaseTestCase : public BaseTestCase {
  Q_OBJECT

 protected:
  ServerConfig getDummyConfig(QString name = "test") {
    ServerConfig dummyConf("127.0.0.1", "",
                           RedisClient::ConnectionConfig ::DEFAULT_REDIS_PORT,
                           name);
    dummyConf.setTimeouts(2000, 2000);
    return dummyConf;
  }
};
