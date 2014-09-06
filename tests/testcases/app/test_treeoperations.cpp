#include "test_treeoperations.h"
#include "models/treeoperations.h"
#include "redisclient/connection.h"

void TestTreeOperations::testCreation()
{
    //given
    QSharedPointer<RedisClient::Connection> connection = getReadyDummyConnection();

    //when
    TreeOperations operations(connection);

    //then
    //all ok
    Q_UNUSED(operations);
}
