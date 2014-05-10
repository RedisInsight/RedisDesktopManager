#ifndef CONNECTIONEXCEPTION_H
#define CONNECTIONEXCEPTION_H

#include <stdexcept>
#include <QString>

namespace RedisClient {

class ConnectionExeption : public std::runtime_error
{
public:
    ConnectionExeption(const QString &err)
        : std::runtime_error(err.toStdString())
    {
    }
};

}
#endif // CONNECTIONEXCEPTION_H
