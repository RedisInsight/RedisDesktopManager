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


namespace ConnectionsTree {

class TreeItem {
public:
    TreeItem() {}

    virtual QString getDisplayName() const = 0;

    virtual QByteArray getName() const { return getDisplayName().toUtf8(); }

    virtual QString getIconUrl() const = 0;

    virtual QString getType() const = 0;    

    virtual QList<QSharedPointer<TreeItem>> getAllChilds() const = 0;

    virtual uint childCount(bool recursive = false) const = 0;

    virtual QSharedPointer<TreeItem> child(uint row) const = 0;

    virtual QWeakPointer<TreeItem> parent() const = 0;

    virtual bool supportChildItems() const { return true; }

    virtual QVariant metadata(const QString&) { return QVariant(); }

    virtual void setMetadata(const QString&, QVariant) {}

    virtual int row() const
    {
        if (!parent())
            return 0;

        auto p = parent().toStrongRef();

        for (uint index = 0; index < p->childCount(); ++index)
        {
            if (p->child(index).data() == this)
                return index;
        }

        return 0;
    }    

    virtual void handleEvent(QString event)
    {
        if (!m_eventHandlers.contains(event))
            return;

        try {
            m_eventHandlers[event]();
        } catch (...) {
            qWarning() << "Error on event processing: " << event;
        }
    }

    virtual bool isLocked() const { return false; }

    virtual bool isEnabled() const = 0;

    virtual ~TreeItem() {}

protected:
    QHash<QString, std::function<void()>> m_eventHandlers;
};

}
