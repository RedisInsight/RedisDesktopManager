#pragma once
#include "basetestcase.h"
#include <QFile>

class TestDialogs : public BaseTestCase
{
    Q_OBJECT
private slots:
    void init();
    void testConnectionDialog();
    void testConnectionDialogValidation();
    void testOkButtonInvalidSettings();
    void testOkButton();
};
