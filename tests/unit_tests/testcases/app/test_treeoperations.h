#pragma once

#include "respbasetestcase.h"

class TestTreeOperations : public RESPBaseTestCase
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

