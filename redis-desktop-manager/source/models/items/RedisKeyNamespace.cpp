#include "RedisKeyNamespace.h"

RedisKeyNamespace::RedisKeyNamespace(QString name, const QIcon & icon)
    : ItemWithNaturalSort(icon, name)

{
    setEditable(false);
}

