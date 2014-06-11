#pragma once

#include <QString>

class AbstractFormatter
{
public:

    virtual ~AbstractFormatter() {};

    virtual void setSource(const QString&);        

    virtual QString getFormatted() = 0;

    virtual bool isValid() = 0;

    virtual QString getRaw() = 0;

    enum FormatterType
    {
        Plain = 0, Json = 1, PHPSerializer = 2
    };

    static AbstractFormatter * getFormatter(FormatterType type = AbstractFormatter::Plain);    

protected:
    QString rawValue;
};

