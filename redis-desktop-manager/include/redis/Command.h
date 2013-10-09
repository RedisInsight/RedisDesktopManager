#pragma once

#include <QtCore>

class Command
{
public:
	/** @see http://redis.io/topics/protocol for more info **/	
	static QString getFormatted(QString command);
};

