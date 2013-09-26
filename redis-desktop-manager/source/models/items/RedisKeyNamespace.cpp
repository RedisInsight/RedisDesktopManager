#include "RedisKeyNamespace.h"

RedisKeyNamespace::RedisKeyNamespace(QString name)
{
	setIcon(QIcon(":/images/namespace.png"));
	setText(name);
	setEditable(false);
}

