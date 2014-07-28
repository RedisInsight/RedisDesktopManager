#pragma once

#include "operations.h"

class ItemOperationsMock : public ConnectionsTree::Operations {

public:

    DatabaseList databases;

    virtual void getDatabases(std::function<void(DatabaseList)> callback) {
        callback(databases);
    }

    RawKeysList keys;
    virtual void getDatabaseKeys(uint dbIndex, std::function<void(const RawKeysList&)> callback) {
        callback(keys);
    }

    virtual QSharedPointer<ConsoleOperations> getConsoleOperations()
    {
        return QSharedPointer<ConsoleOperations>();
    }

    QString namespaceSeparator = ":";
    QString getNamespaceSeparator() { return namespaceSeparator; }

    void disconnect() {}
};
