#pragma once

#include "console/operations.h"

class ConsoleOperationsMock : public Console::Operations
{
public:
    virtual void init() override {}
    virtual QString getConsoleName() override { return "test_console"; }

public slots:
    virtual void executeCommand(const QString&) override {}
};
