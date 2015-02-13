#pragma once

#include "basetestcase.h"

class TestTreeOperations : public BaseTestCase
{
    Q_OBJECT    

private slots:
    void testCreation();           
    void testGetDatabaseKeys();
    void testGetDatabaseKeys_data();
};

