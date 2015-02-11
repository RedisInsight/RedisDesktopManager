#pragma once
#include "command.h"

namespace RedisClient {

class ScanCommand : public Command
{
public:
    ScanCommand(const QString& cmd) : Command(cmd) {}
    void setCursor(int cursor);

    static bool isValidScanCommand(Command r);
private:
    static bool isKeyScanCommand(const QString& cmd);
    static bool isValueScanCommand(const QString& cmd);
};


}
