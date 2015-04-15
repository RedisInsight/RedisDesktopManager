#pragma once
#include "connections-tree/operations.h"
#include "connections-tree/items/treeitem.h"

class ItemOperationsMock : public ConnectionsTree::Operations {

public:

    DatabaseList databases;

    virtual void getDatabases(std::function<void(DatabaseList)> callback) {
        callback(databases);
    }

    RawKeysList keys;
    virtual void getDatabaseKeys(uint, std::function<void(const RawKeysList&)> callback) {
        callback(keys);
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
    void openNewKeyDialog(int, QString = QString()) override {}
};

class DummyParentView : public ConnectionsTree::TreeItem::ParentView {

public:
    QWidget* getParentWidget() override
    {
        return nullptr;
    }
};
