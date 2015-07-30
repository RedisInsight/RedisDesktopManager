#pragma once
#include "treeitem.h"
#include "connections-tree/operations.h"
#include <QList>
#include <QObject>

namespace ConnectionsTree {

    class Model;

    class ServerItem : public QObject, public TreeItem
    {
        Q_OBJECT
    public:
        ServerItem(const QString& name, QSharedPointer<Operations> operations,
                   const Model& model);
        ~ServerItem();

        QString getDisplayName() const override;
        QIcon getIcon() const override;
        QList<QSharedPointer<TreeItem>> getAllChilds() const override;
        uint childCount() const override;
        QSharedPointer<TreeItem> child(uint row) const override;
        QWeakPointer<TreeItem> parent() const override;

        int row() const override;
        void setRow(int r);

        bool onClick(ParentView& treeView) override;
        QSharedPointer<QMenu> getContextMenu(ParentView& treeView) override;

        bool isLocked() const override;
        bool isEnabled() const override;
        bool isDatabaseListLoaded() const;

        void load();
        void unload();
        void reload();

        void setName(const QString &name);
        void setWeakPointer(QWeakPointer<ServerItem>);
    signals:
        void error(const QString&);
        void databaseListLoaded();
        void unloadStarted();
        void editActionRequested();
        void deleteActionRequested();
        void updateIcon();
        void updateDbIcon(unsigned int dbIndex);
        void keysLoadedInDatabase(unsigned int dbIndex);
        void unloadStartedInDatabase(unsigned int dbIndex);

    private:
        QString m_name;
        bool m_locked;
        bool m_databaseListLoaded;
        int m_row;
        QSharedPointer<Operations> m_operations;
        QList<QSharedPointer<TreeItem>> m_databases;
        const Model& m_model;
        QWeakPointer<ServerItem> m_self;
    };
}
