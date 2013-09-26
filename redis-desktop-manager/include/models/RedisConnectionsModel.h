#ifndef REDISCONNECTIONSMODEL_H
#define REDISCONNECTIONSMODEL_H

#include <QStandardItemModel>
#include "RedisConnectionAbstract.h"

class RedisConnectionsModel : public QStandardItemModel
{
	Q_OBJECT

public:
	RedisConnectionsModel(QObject *parent);
	~RedisConnectionsModel();

	void loadKeys(RedisConnectionAbstract *);
};

#endif // REDISCONNECTIONSMODEL_H
