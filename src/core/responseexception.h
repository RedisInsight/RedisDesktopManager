#ifndef RESPONSEEXCEPTION_H
#define RESPONSEEXCEPTION_H

#include <QString>
#include "connectionexception.h"

namespace RedisClient {

class ResponseException : public ConnectionExeption
{
public:
    ResponseException(const QString &err)
        : ConnectionExeption(err)
    {
    }
};

}

#endif // RESPONSEEXCEPTION_H
