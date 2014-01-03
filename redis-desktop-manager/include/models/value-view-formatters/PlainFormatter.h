#pragma once

#include "AbstractFormatter.h"

class PlainFormatter : public AbstractFormatter
{
public:
    QString getFormatted();    

    bool isValid();

    QString getRaw();
};

