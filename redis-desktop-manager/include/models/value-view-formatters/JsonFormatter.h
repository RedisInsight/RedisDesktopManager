#pragma once
#include "AbstractFormatter.h"

class JsonFormatter : public AbstractFormatter
{
public:
	QString getFormatted();
};

