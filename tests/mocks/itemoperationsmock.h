#pragma once

#include "operations.h"

class ItemOperationsMock : public ConnectionsTree::Operations {

public:

    DatabaseList databases;

    virtual void getDatabases(std::function<void(DatabaseList)> callback) {
        callback(databases);
    }

    void disconnect() {}
};
