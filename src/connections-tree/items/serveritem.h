#ifndef SERVERITEM_H
#define SERVERITEM_H

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
        ServerItem(const QString& name, QSharedPointer<Operations> operations, const Model& model);
        ~ServerItem();

        QString getDisplayName() const override;
        QIcon getIcon() const override;
        QList<QSharedPointer<TreeItem>> getAllChilds() const override;
        uint childCount() const override;
        QSharedPointer<TreeItem> child(int row) const override;
        const TreeItem* parent() const override;

        bool onClick(ParentView& treeView, TabWidget& tabs) override;
        void onWheelClick(ParentView& treeView, TabWidget& tabs) override {}
        QSharedPointer<QMenu> getContextMenu(ParentView& treeView, TabWidget& tabs) override;

        bool isLocked() const override;
        bool isDatabaseListLoaded() const;

        void load();
        void unload();
        void reload();

    signals:
        void databaseListLoaded();
        void editActionRequested();
        void deleteActionRequested();
        void keysLoadedInDatabase(unsigned int dbIndex);

        /*signals:
    void keysLoaded(unsigned int dbIndex);*/

    private:
        QString m_name;
        bool m_locked;        
        bool m_databaseListLoaded;
        QSharedPointer<Operations> m_operations;
        QList<QSharedPointer<TreeItem>> m_databases;
        const Model& m_model;
    };
}

#endif // SERVERITEM_H
