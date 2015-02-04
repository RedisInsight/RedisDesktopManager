#include "test_treeoperations.h"
#include "models/treeoperations.h"
#include "redisclient/connection.h"
#include "app/widgets/consoletabs.h"

void TestTreeOperations::testCreation()
{
    //given
    ConsoleTabs tabsWidget;
    QSharedPointer<RedisClient::Connection> connection = getReadyDummyConnection();

    //when
    TreeOperations operations(connection, tabsWidget);

    //then
    //all ok
    Q_UNUSED(operations);
}
