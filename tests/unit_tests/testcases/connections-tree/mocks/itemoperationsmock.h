#pragma once
#include "connections-tree/operations.h"
#include "connections-tree/items/treeitem.h"

class ItemOperationsMock : public ConnectionsTree::Operations {

public:
    ItemOperationsMock(bool positive_mode=true):
        m_positive_mode(positive_mode) {}

    QMap<int, int> databases;
    virtual void getDatabases(std::function<void(QMap<int, int>)> callback) {
        if (m_positive_mode)
            callback(databases);
        else
            throw ConnectionsTree::Operations::Exception("fake error");
    }

    QList<QByteArray> keys;
    void getDatabaseKeys(uint, QString, std::function<void(const QList<QByteArray>&, const QString&)> callback) override {
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

    virtual void deleteDbNamespace(ConnectionsTree::NamespaceItem&) override {}

    virtual void flushDb(int, std::function<void(const QString&)>) override {}

    virtual QString mode() { return QString("fake"); }

protected:
    bool m_positive_mode;
};
