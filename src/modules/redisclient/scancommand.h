#pragma once
#include "command.h"

namespace RedisClient {

class ScanCommand : public Command
{
public:
    void setCursor(int cursor);

    static bool isValidScanCommand(Command r);
};


}
