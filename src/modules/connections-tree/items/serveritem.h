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
        ServerItem(const QString& name, QSharedPointer<Operations> operations,
                   const Model& model);
        ~ServerItem();

        QString getDisplayName() const override;
        QIcon getIcon() const override;
        QList<QSharedPointer<TreeItem>> getAllChilds() const override;
        uint childCount() const override;
        QSharedPointer<TreeItem> child(int row) const override;
        const TreeItem* parent() const override;

        int row() const override;
        void setRow(int r);

        bool onClick(ParentView& treeView) override;
        void onWheelClick(ParentView& treeView) override {}
        QSharedPointer<QMenu> getContextMenu(ParentView& treeView) override;

        bool isLocked() const override;
        bool isDatabaseListLoaded() const;

        void load();
        void unload();
        void reload();

    signals:
        void databaseListLoaded();
        void unloadStarted();
        void editActionRequested();
        void deleteActionRequested();
        void keysLoadedInDatabase(unsigned int dbIndex);

        /*signals:
    void keysLoaded(unsigned int dbIndex);*/

    private:
        QString m_name;
        bool m_locked;        
        bool m_databaseListLoaded;
        int m_row;
        QSharedPointer<Operations> m_operations;
        QList<QSharedPointer<TreeItem>> m_databases;
        const Model& m_model;
    };
}

#endif // SERVERITEM_H
