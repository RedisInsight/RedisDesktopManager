#pragma once
#include <functional>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QVariantMap>
#include <QString>
#include <QIcon>
#include <QList>
#include <QHash>
#include <QVariantMap>
#include <QDebug>
#include <QModelIndex>


namespace ConnectionsTree {

class Model;

class TreeItem {
public:
    TreeItem(Model& m);

    virtual ~TreeItem() {}

    virtual QString getDisplayName() const = 0;

    virtual QByteArray getName() const { return getDisplayName().toUtf8(); }

    virtual QByteArray getFullPath() const { return QByteArray(); }

    virtual QString getIconUrl() const = 0;

    virtual QString getType() const = 0;

    virtual int itemDepth() const = 0;

    virtual QList<QSharedPointer<TreeItem>> getAllChilds() const = 0;

    virtual uint childCount(bool recursive = false) const = 0;

    virtual QSharedPointer<TreeItem> child(uint row) const = 0;

    virtual QWeakPointer<TreeItem> parent() const = 0;

    virtual bool supportChildItems() const { return true; }

    virtual QVariant metadata(const QString&) { return QVariant(); }

    virtual void setMetadata(const QString&, QVariant) {}

    virtual int row() const;

    virtual QWeakPointer<TreeItem> getSelf();

    virtual void handleEvent(QString event);

    virtual bool isLocked() const { return m_locked; }

    virtual bool isEnabled() const = 0;

    virtual bool isExpanded() const { return false; }

    virtual bool canFetchMore() const { return false; }

    virtual void fetchMore() {}

    virtual Model& model();

protected:
    void lock();
    void unlock();

protected:
    Model& m_model;
    QHash<QString, std::function<void()>> m_eventHandlers;

private:
    QWeakPointer<TreeItem> m_selfPtr;
    bool m_locked;
};

typedef QList<QSharedPointer<TreeItem>> TreeItems;

}
