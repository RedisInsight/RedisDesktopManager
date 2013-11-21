#include "JsonFormatter.h"

#include <json\json.h>

QString JsonFormatter::getFormatted()
{
	Json::Value root;   
	Json::Reader reader;
	bool parsingSuccessful = reader.parse( rawValue.toStdString(), root );

	if (!parsingSuccessful)
	{
		return QString("Invalid JSON");
	}

	Json::StyledWriter writer;		

	return QString::fromStdString(writer.write(root));
}