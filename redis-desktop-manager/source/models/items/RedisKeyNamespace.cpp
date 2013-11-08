#include "RedisKeyNamespace.h"

RedisKeyNamespace::RedisKeyNamespace(QString name, QIcon & icon)
	: QStandardItem(icon, name)

{
	setEditable(false);
}

