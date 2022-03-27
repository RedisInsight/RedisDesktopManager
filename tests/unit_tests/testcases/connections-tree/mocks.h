#pragma once
#include <fakeit.hpp>

#include "connections-tree/operations.h"

using namespace fakeit;

Mock<ConnectionsTree::Operations> getOperations();

Mock<ConnectionsTree::Operations> getOperationsWithGetDatabases(
    RedisClient::DatabaseList db, const QString& err);

Mock<ConnectionsTree::Operations> getOperationsWithDbAndKeys(
    RedisClient::DatabaseList db, const QString& err, QList<QByteArray> keys);
