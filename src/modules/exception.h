#pragma once

#include <stdexcept>
#include <QString>

#define ADD_EXCEPTION \
public: struct Exception : public std::runtime_error { \
    Exception(const QString &err) : std::runtime_error(err.toStdString()) {} \
};
