#pragma once

#include "basetestcase.h"

class TestTreeOperations : public BaseTestCase
{
    Q_OBJECT    

private slots:
    void testCreation();
    
    void testGetDatabases();
    
    void testLoadNamespaceItems();
    void testLoadNamespaceItems_data();

    void testFlushDb();
    void testFlushDbCommandError();
};

