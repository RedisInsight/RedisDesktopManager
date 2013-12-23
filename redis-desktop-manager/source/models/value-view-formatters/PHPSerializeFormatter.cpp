#include "PHPSerializeFormatter.h"

#include <QStringList>
#include <QDebug>

PHPSerializeFormatter::PHPSerializeFormatter()
{
    arrayItemWithValueRegex.setPatternSyntax(QRegExp::RegExp2);

    arrayItemWithValueRegex.setPattern("[is]:*[\\d]*:(\\d+|\".+\");" //array key - integer or string        
        "(s|i|d|b|a|O|R|r|N):{0,1}" //value type
        "(\\d+:)*"        // [string length] - skip
        "("        
         "\".+\""                    // value - string
        "|[0-9\\.]+"                // value - int, double, bool or repeat block
        "|\\{.*\\}"                    // value - array
        "|\".+\":\\d+:\\{.+\\}"        // value - object    
        ")*"
        ";*" // simple types ends with ";"
        ); 

    //qDebug() << "Pattern: " << arrayItemWithValueRegex.pattern();
}

QString PHPSerializeFormatter::getFormatted()
{
    //todo implement this

    switch (getType(rawValue))
    {
        case Null:
            return "NULL";                

        case Integer:
        case Double:        
            return parseDecimal(rawValue);

        case Boolean:
            return (parseDecimal(rawValue) == "1")? "true" : "false";

        case String:
            return parseString(rawValue);

        case Array:
            return parseArray(rawValue);

        case Object:
            return parseObject(rawValue);

        case Invalid:
        default:
            return "Invalid or corrupted serialize string";
    }    
}

PHPSerializeFormatter::ValueType PHPSerializeFormatter::getType(const QChar &typeChar)
{
    switch (typeChar.toLatin1())
    {
        case 'N': return Null;
        case 'i': return Integer;
        case 'b': return Boolean;
        case 'd': return Double;
        case 's': return String;
        case 'a': return Array;
        case 'r':
        case 'R': return RepeatBlock;
        case 'O':
        case 'o': return Object;
        default: return Invalid; 
    }
}

PHPSerializeFormatter::ValueType PHPSerializeFormatter::getType(const QString &rawValue)
{
    //quick validation
    if (rawValue.size() < 2) {
        return Invalid;
    }

    return getType(rawValue.at(0));
}

QString PHPSerializeFormatter::parseDecimal(const QString &rawValue)
{
    // Example:
    // char numbers: 012345
    // string:       i:123;
    // 2 - is start position of value
    // 3 - is type prefix + removed char ';'
    return rawValue.mid(2, rawValue.size() - 3);
}

QString PHPSerializeFormatter::parseString(const QString &rawValue)
{
    int valueStartsFrom = rawValue.indexOf(':', 2) + 1;

    return rawValue.mid(valueStartsFrom, rawValue.size() - valueStartsFrom - 1);
}

QString PHPSerializeFormatter::parseArray(const QString &rawValue, int whiteSpaceSize)
{
    QString key;
    ValueType type;
    QString value;
    QString result;
    QStringList parsedResult;
    QString whitespace = QString(" ").repeated(whiteSpaceSize);

    int pos = 0;

    while ((pos = arrayItemWithValueRegex.indexIn(rawValue, pos)) != -1) {        

        pos += arrayItemWithValueRegex.matchedLength();

        //qDebug() << "Cap count: " << arrayItemWithValueRegex.captureCount() << "texts \n" << arrayItemWithValueRegex.capturedTexts().join('\n');

        if (arrayItemWithValueRegex.captureCount() < captionsCount 
            || (arrayItemWithValueRegex.cap(Value).isEmpty() 
                && arrayItemWithValueRegex.cap(TypeChar).at(0) != 'N'))  {            
            break;
        }

        key =  arrayItemWithValueRegex.cap(Key);
        type = getType(arrayItemWithValueRegex.cap(TypeChar).at(0));

        if (type == Invalid)
            break;
        
        if (type == Array) {

            value = parseArray(arrayItemWithValueRegex.cap(Value), 
                whiteSpaceSize + whiteSpaceDefaultSize);

        } else if (type == Boolean) {

            value = (value == "1") ? "true" : "false";

        } else if (type == Null) {

            value = "NULL";

        } else if (type == RepeatBlock) {

            value = QString("[Repeat block:%1] *** Not fully supported yet ***").arg(value);

        } else if (type == Object) {

            value = parseObject(QString("O:0:%1").arg(arrayItemWithValueRegex.cap(Value)));

        } else {

            value = arrayItemWithValueRegex.cap(Value);

        }

        result = QString("%1[%2] => %3")
            .arg(whitespace).arg(key).arg(value);

        parsedResult << result;        
    }    

    return QString("array(\n%1\n%2)")
        .arg(parsedResult.join(",\n"))
        .arg(QString(" ").repeated(whiteSpaceSize-whiteSpaceDefaultSize));
}

QString PHPSerializeFormatter::parseObject(const QString &rawValue)
{
    QRegExp getObjectClass("O:\\d+:\"(.+)\":\\d+:\\{");

    if (getObjectClass.indexIn(rawValue) == -1) {
        return "Invalid serialized string";
    }

    return QString("Object:%1").arg(getObjectClass.cap(1));
}
