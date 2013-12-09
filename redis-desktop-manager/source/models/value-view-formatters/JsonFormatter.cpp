#include "JsonFormatter.h"

#include <QJsonDocument>

QString JsonFormatter::getFormatted()
{
	QJsonParseError * parsingError = new QJsonParseError;
	QJsonDocument document = QJsonDocument::fromJson(rawValue.toStdString().c_str(), parsingError);

	if (parsingError->error != QJsonParseError::NoError) 
	{
		return QString("Invalid JSON");
	}

	return QString(document.toJson(QJsonDocument::Indented));
}
