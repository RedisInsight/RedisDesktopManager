#pragma once

#include "basetestcase.h"

class TestTreeOperations : public BaseTestCase
{
    Q_OBJECT    

private slots:
    void testCreation();  
    
    /**
     * TODO(u_glide): Fix freeze in this tests
     */ 
    void testGetDatabases();
    void testGetDatabasesWithSelectScan();
    
    void testGetDatabaseKeys();
    void testGetDatabaseKeys_data();
};

