#pragma once
#include "command.h"

namespace RedisClient {

class ScanCommand : public Command
{
public:
    ScanCommand(const QStringList& cmd, QObject* owner, int db) : Command(cmd, owner, db) {}
    ScanCommand(const QStringList& cmd) : Command(cmd) {}

    void setCursor(int cursor);

    bool isValidScanCommand();

private:
    bool isKeyScanCommand(const QString& cmd);
    bool isValueScanCommand(const QString& cmd);
};

}
