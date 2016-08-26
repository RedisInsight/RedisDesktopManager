#pragma once
#include "connections-tree/operations.h"
#include "connections-tree/items/treeitem.h"

class ItemOperationsMock : public ConnectionsTree::Operations {

public:
    ItemOperationsMock(bool positive_mode=true):
        m_positive_mode(positive_mode) {}

    DatabaseList databases;
    virtual void getDatabases(std::function<void(DatabaseList)> callback) {
        if (m_positive_mode)
            callback(databases);
        else
            throw ConnectionsTree::Operations::Exception("fake error");
    }

    RawKeysList keys;
    void getDatabaseKeys(uint, std::function<void(const RawKeysList&, const QString&)> callback) override {
        if (m_positive_mode)
            callback(keys, QString());
        else
            throw ConnectionsTree::Operations::Exception("fake error");
    }

    virtual QSharedPointer<Console::Operations> getConsoleOperations()
    {
        return QSharedPointer<Console::Operations>();
    }

    QString namespaceSeparator = ":";
    QString getNamespaceSeparator() { return namespaceSeparator; }

    void disconnect() {}

    virtual void openKeyTab(ConnectionsTree::KeyItem&, bool) override {}
    virtual void openConsoleTab() override {}
    void openNewKeyDialog(int, std::function<void()>, QString = QString()) override {}
    virtual void notifyDbWasUnloaded(int) override {}
    virtual void deleteDbKey(ConnectionsTree::KeyItem&, std::function<void(const QString&)>) override {}

protected:
    bool m_positive_mode;
};
