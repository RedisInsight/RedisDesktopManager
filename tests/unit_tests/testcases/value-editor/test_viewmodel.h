#pragma once

#include "basetestcase.h"

class TestViewModel : public BaseTestCase
{
    Q_OBJECT

private slots:
    // Tests for QML exposed functions
    void testAddKey();

   // Tests for c++ only functions
    void testOpenTab();

    // Test model
    void testAbstractModelMethods();
};
