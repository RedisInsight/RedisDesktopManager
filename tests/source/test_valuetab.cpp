#include "test_valuetab.h"
#include "valueViewTab.h"

void TestValueTab::testCreationOfObject()
{
    //given
    RedisKeyItem * key = new RedisKeyItem;

    //when
    ValueTab * tab = new ValueTab(key);
    tab->close();


    //then
    // no errors
}


