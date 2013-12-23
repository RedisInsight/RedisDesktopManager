#include "AbstractFormatter.h"
#include "PlainFormatter.h"
#include "PHPSerializeFormatter.h"
#include "JsonFormatter.h"

void AbstractFormatter::setRawValue(const QString&r)
{
    rawValue = r;
}

AbstractFormatter * AbstractFormatter::getFormatter(FormatterType type)
{
    switch (type)
    {                
        case AbstractFormatter::Json:
            return new JsonFormatter;

        case AbstractFormatter::PHPSerializer:
            return new PHPSerializeFormatter;    

        case AbstractFormatter::Plain:
        default:
            return new PlainFormatter;
    }
}