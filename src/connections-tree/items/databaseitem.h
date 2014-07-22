#ifndef DATABASEITEM_H
#define DATABASEITEM_H

#include "treeitem.h"
#include "connections-tree/operations.h"
#include <QtConcurrent>

namespace ConnectionsTree {

class NamespaceItem;

class DatabaseItem : public TreeItem
{
public:
    DatabaseItem(const QString& displayName, unsigned int index, int keysCount, QSharedPointer<Operations> operations);


    QString getDisplayName() const;
    QIcon getIcon() const;
    QList<QSharedPointer<TreeItem>> getAllChilds() const;
    uint childCount() const;
    QSharedPointer<TreeItem> child(int row) const;
    QSharedPointer<TreeItem> parent() const;

    bool onClick(QWeakPointer<ParentView> treeView, QWeakPointer<QTabWidget> tabs);
    void onWheelClick(QWeakPointer<ParentView> treeView, QWeakPointer<QTabWidget> tabs);
    QSharedPointer<QMenu> getContextMenu(QWeakPointer<ParentView> treeView, QWeakPointer<QTabWidget> tabs);

    bool isLocked() const {return false;}

private:
    QString m_name;
    unsigned int m_index;
    int m_keysCount;
    bool m_locked;
    QSharedPointer<Operations> m_operations;
    QList<QSharedPointer<TreeItem>> m_keys;

    class KeysTreeRenderer
    {
    public:
        QList<QSharedPointer<TreeItem>> renderKeys(QSharedPointer<Operations> operations,
                                                   const Operations::RawKeysList& keys,
                                                   const QRegExp& filter,
                                                   const QString& namespaceSeparator);

    private:
         QSharedPointer<Operations> m_operations;
         QList<QSharedPointer<TreeItem> >  m_result;
         QString m_namespaceSeparator;

         void renderNamaspacedKey(QSharedPointer<NamespaceItem> currItem,
                                  const QString& notProcessedKeyPart,
                                  const QString& fullKey);
    };
};

}

#endif // DATABASEITEM_H
