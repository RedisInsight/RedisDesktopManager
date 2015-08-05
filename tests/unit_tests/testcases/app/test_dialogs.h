#pragma once
#include "basetestcase.h"

class TestDialogs : public BaseTestCase
{
    Q_OBJECT

private slots:
    void testConnectionDialog();
    void testConnectionDialogValidation();
    void testOkButtonInvalidSettings();
    void testOkButton();
};
