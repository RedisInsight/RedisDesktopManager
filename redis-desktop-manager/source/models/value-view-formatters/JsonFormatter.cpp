#include "JsonFormatter.h"

#include <QScopedPointer>
#include <QJsonDocument>

JsonFormatter::JsonFormatter()
    : valid(false)
{
}

void JsonFormatter::setSource(const QString &src)
{
    rawValue = src;

    QScopedPointer<QJsonParseError> parsingError(new QJsonParseError);
    document = QJsonDocument::fromJson(rawValue.toStdString().c_str(), parsingError.data());
    valid = (parsingError->error == QJsonParseError::NoError);
}

QString JsonFormatter::getFormatted()
{
    if (!isValid()) 
    {
        return QString("Invalid JSON");
    }

    return QString(document.toJson(QJsonDocument::Indented));
}

bool JsonFormatter::isValid()
{
    return valid;
}

QString JsonFormatter::getRaw()
{
    if (!isValid()) 
    {
        return QString("Invalid JSON");
    }

    return QString(document.toJson(QJsonDocument::Compact));
}