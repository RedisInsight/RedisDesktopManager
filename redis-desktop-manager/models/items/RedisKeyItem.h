#ifndef REDISKEYITEM_H
#define REDISKEYITEM_H

#include "ItemWithNaturalSort.h"

class RedisServerDbItem;
class KeyModel;

namespace RedisClient {
class Command;
class Connection;
}

class RedisKeyItem : public ItemWithNaturalSort
{    
public:

    const static int TYPE = 2200;

    RedisKeyItem();

    RedisKeyItem(QString name, RedisServerDbItem * db, const QIcon &);

    void init(QString name, RedisServerDbItem * db, const QIcon &);

    QString getTabLabelText();

    int virtual type() const;

    KeyModel * getKeyModel(const QString&);

    RedisClient::Command getTypeCommand();

    RedisClient::Connection * getConnection();

    void remove();

    RedisServerDbItem * getDbItem();

private:    
    RedisServerDbItem * db;    
};

#endif // REDISKEYITEM_H
