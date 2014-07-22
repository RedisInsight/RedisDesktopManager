#include "databaseitem.h"
#include "namespaceitem.h"
#include "keyitem.h"
#include "KeysTreeRenderer.h"
#include <typeinfo>

using namespace ConnectionsTree;

DatabaseItem::DatabaseItem(const QString& displayName, unsigned int index, int keysCount, QSharedPointer<Operations> operations)
    : m_name(displayName),
      m_index(index),
      m_keysCount(keysCount),
      m_locked(false),
      m_operations(operations)
{
}

QString DatabaseItem::getDisplayName() const
{
    return m_name;
}

QIcon DatabaseItem::getIcon() const
{
    return QIcon();
}

QList<QSharedPointer<TreeItem> > DatabaseItem::getAllChilds() const
{
    return QList<QSharedPointer<TreeItem>>();
}

uint DatabaseItem::childCount() const
{
    return 0;
}

QSharedPointer<TreeItem> DatabaseItem::child(int row) const
{
    return QSharedPointer<TreeItem>();
}

QSharedPointer<TreeItem> DatabaseItem::parent() const {
    return QSharedPointer<TreeItem>();
}

bool DatabaseItem::onClick(QWeakPointer<ParentView> treeView, QWeakPointer<QTabWidget> tabs)
{
    Q_UNUSED(treeView);
    Q_UNUSED(tabs);

    loadKeys();

    return true;
}

void DatabaseItem::onWheelClick(QWeakPointer<TreeItem::ParentView> treeView, QWeakPointer<QTabWidget> tabs)
{
    Q_UNUSED(treeView);
    Q_UNUSED(tabs);
}

QSharedPointer<QMenu> DatabaseItem::getContextMenu(QWeakPointer<TreeItem::ParentView> treeView, QWeakPointer<QTabWidget> tabs)
{
    return QSharedPointer<QMenu>();
}

void DatabaseItem::loadKeys()
{
    if (m_keys.size() > 0)
        return;

    m_locked = true;

    m_operations->getDatabaseKeys(m_index, [this](const Operations::RawKeysList& rawKeys) {
        if (rawKeys.size() == 0) {
            m_locked = false;
            return;
        }

        keysLoadingResult = QtConcurrent::run(KeysTreeRenderer::renderKeys, this, rawKeys, filter, iconStorage, server->connection->getConfig().namespaceSeparator);

        keysLoadingWatcher.setFuture(keysLoadingResult);

    });
}

QList<QSharedPointer<TreeItem> >
DatabaseItem::KeysTreeRenderer::renderKeys(QSharedPointer<Operations> operations,
                                           const Operations::RawKeysList &rawKeys,
                                           const QRegExp &filter,
                                           const QString &namespaceSeparator)
{
    rawKeys.sort();

    m_operations = operations;
    m_result.clear();
    m_namespaceSeparator = namespaceSeparator;

    for (QString rawKey : rawKeys) {

        //if filter enabled - skip keys
        if (!filter.isEmpty() && !rawKey.contains(filter)) {
            continue;
        }

        renderNamaspacedKey(QSharedPointer<TreeItem>(), rawKey, rawKey, m_namespaceSeparator);
    }

    return result;
}

void DatabaseItem::KeysTreeRenderer::renderNamaspacedKey(QSharedPointer<NamespaceItem> currItem,
                                                         const QString &notProcessedKeyPart,
                                                         const QString &fullKey)
{
    if (!notProcessedKeyPart.contains(m_namespaceSeparator)) {

        QSharedPointer<KeyItem> newKey((new KeyItem(fullKey, m_operations)));

        if (currItem.isNull()) root.push_back(newKey);
        else currItem->append(newKey);

        return;
    }

    int indexOfNaspaceSeparator = notProcessedKeyPart.indexOf(namespaceSeparator);

    QString firstNamespaceName = notProcessedKeyPart.mid(0, indexOfNaspaceSeparator);

    QSharedPointer<NamespaceItem> namespaceItem;
    int size = (currItem.isNull())? root.size() : currItem->childCount();

    for (int i=0; i < size; ++i)
    {
        QSharedPointer<TreeItem> child = (currItem.isNull())? m_result[i] : currItem->child(i);

        if (child->getDisplayName() == firstNamespaceName
                && typeid(NamespaceItem)==typeid(*child)) {

                namespaceItem = child;
                break;
        }
    }


    if (namespaceItem.isNull()) {
        namespaceItem = QSharedPointer<NamespaceItem>(new NamespaceItem(firstNamespaceName));

        if (currItem.isNull()) m_result.push_back(namespaceItem);
        else currItem->appendRow(namespaceItem);
    }

    renderNamaspacedKey(namespaceItem, notProcessedKeyPart.mid(indexOfNaspaceSeparator+1), fullKey);
}
