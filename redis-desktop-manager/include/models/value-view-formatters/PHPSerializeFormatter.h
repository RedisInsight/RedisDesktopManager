#pragma once

#include <QRegExp>
#include "AbstractFormatter.h"

class PHPSerializeFormatter : public AbstractFormatter
{

public:

    PHPSerializeFormatter();

    virtual QString getFormatted();

private:

    enum ValueType {
        Invalid, Null, Integer, Double, Boolean, String, Array, Object, RepeatBlock
    };

    // Regex captions map:
    // - array key (int or string)
    // - type char of value
    // - value
    enum RegexCaptions {
        Key = 1, TypeChar = 2, Value = 4
    };

    static const int captionsCount = 4;

    static const int whiteSpaceDefaultSize = 3;

    ValueType getType(const QString &);

    ValueType getType(const QChar &);

    QString parseDecimal(const QString &);

    QString parseString(const QString &);

    QRegExp arrayItemWithValueRegex;

    QString parseArray(const QString &, int whiteSpaceSize = whiteSpaceDefaultSize);

    QString parseObject(const QString &);

};


