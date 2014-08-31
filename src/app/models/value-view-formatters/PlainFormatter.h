#pragma once

#include "abstractformatter.h"

class PlainFormatter : public AbstractFormatter
{
public:
    QString getFormatted();    

    bool isValid();

    QString getRaw();
};

