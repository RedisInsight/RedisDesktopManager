#ifndef DATABASEITEM_H
#define DATABASEITEM_H

#include "treeitem.h"
#include "connections-tree/operations.h"
#include <QtConcurrent>

namespace ConnectionsTree {

class NamespaceItem;

class DatabaseItem : public QObject, public TreeItem
{
    Q_OBJECT

public:
    DatabaseItem(const QString& displayName, unsigned int index, int keysCount,
                 QSharedPointer<Operations> operations, QWeakPointer<TreeItem> parent);

    ~DatabaseItem();

    QString getDisplayName() const override;
    QIcon getIcon() const override;
    QList<QSharedPointer<TreeItem>> getAllChilds() const override;
    uint childCount() const override;
    QSharedPointer<TreeItem> child(uint row) const override;
    QWeakPointer<TreeItem> parent() const override;

    bool onClick(ParentView& treeView) override;
    void onWheelClick(ParentView& treeView) override;
    QSharedPointer<QMenu> getContextMenu(ParentView& treeView) override;

    bool isLocked() const override {return false;}
    bool isEnabled() const override {return true;}

    void loadKeys();
    void unload();
    int getIndex() const;

signals:
    void keysLoaded(unsigned int dbIndex);
    void unloadStarted(unsigned int dbIndex);
    void updateIcon(unsigned int dbIndex);

protected slots:
    void onKeysRendered();

protected:
    void reload();
    void filterKeys(const QRegExp& filter);
    void resetFilter();
    void renderRawKeys(const Operations::RawKeysList& rawKeys);

private:
    class KeysTreeRenderer
    {
    public:        

        QList<QSharedPointer<TreeItem>> renderKeys(QSharedPointer<Operations> operations,
                                                   Operations::RawKeysList keys,
                                                   QRegExp filter,
                                                   QString namespaceSeparator,
                                                   QSharedPointer<DatabaseItem>);

    private:                  
         void renderNamaspacedKey(QSharedPointer<NamespaceItem> currItem,
                                  const QString& notProcessedKeyPart,
                                  const QString& fullKey,
                                  QSharedPointer<Operations> operations,
                                  const QString& namespaceSeparator,
                                  QList<QSharedPointer<TreeItem>>& m_result,
                                  QSharedPointer<DatabaseItem>
                                  );
    };

private:
    QString m_name;
    unsigned int m_index;
    int m_keysCount;
    bool m_locked;
    QSharedPointer<Operations> m_operations;
    QList<QSharedPointer<TreeItem>> m_keys;
    QFutureWatcher<QList<QSharedPointer<TreeItem>>> m_keysLoadingWatcher;
    KeysTreeRenderer m_keysRenderer;
    QWeakPointer<TreeItem> m_parent;
    Operations::RawKeysList m_rawKeys;
    QRegExp m_filter;
};

}

#endif // DATABASEITEM_H
