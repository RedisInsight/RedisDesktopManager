#pragma once

#include "AbstractFormatter.h"

class PHPSerializeFormatter : public AbstractFormatter
{

public:
	virtual QString getFormatted();
	
};

