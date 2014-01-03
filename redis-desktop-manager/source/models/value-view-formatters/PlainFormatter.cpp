#include "PlainFormatter.h"


QString PlainFormatter::getFormatted()
{
    return rawValue;
}

bool PlainFormatter::isValid()
{
    return true;
}

QString PlainFormatter::getRaw()
{
    return rawValue;
}