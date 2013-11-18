#include "RedisKeyNamespace.h"

RedisKeyNamespace::RedisKeyNamespace(QString name, QIcon & icon)
	: ItemWithNaturalSort(icon, name)

{
	setEditable(false);
}

