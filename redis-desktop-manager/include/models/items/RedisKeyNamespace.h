#ifndef REDISKEYNAMESPACE_H
#define REDISKEYNAMESPACE_H

#include "ItemWithNaturalSort.h"

class RedisKeyNamespace : public ItemWithNaturalSort
{
public:

	const static int TYPE = 2300;

	RedisKeyNamespace(QString name, QIcon & icon);		

	int virtual type() const
	{
		return TYPE;
	}

private:
	
};

#endif // REDISKEYNAMESPACE_H
