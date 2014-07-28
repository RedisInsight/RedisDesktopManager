#include "databaseitem.h"
#include "namespaceitem.h"
#include "keyitem.h"
#include <typeinfo>
#include <functional>

using namespace ConnectionsTree;

DatabaseItem::DatabaseItem(const QString& displayName,
                           unsigned int index, int keysCount,
                           QSharedPointer<Operations> operations,
                           const TreeItem* parent)
    : m_name(displayName),
      m_index(index),
      m_keysCount(keysCount),
      m_locked(false),
      m_operations(operations)
{    
    QObject::connect(&m_keysLoadingWatcher, SIGNAL(finished()), // &QFutureWatcherBase::finished,
                     this, SLOT(onKeysRendered()));
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
    return m_keys;
}

uint DatabaseItem::childCount() const
{
    return m_keys.size();
}

QSharedPointer<TreeItem> DatabaseItem::child(int row) const
{
    if (0 <= row && row < childCount())
        return m_keys.at(row);

    return QSharedPointer<TreeItem>();
}

const TreeItem *DatabaseItem::parent() const
{
    return nullptr;
}

bool DatabaseItem::onClick(ParentView& treeView, QWeakPointer<QTabWidget> tabs)
{
    Q_UNUSED(treeView);
    Q_UNUSED(tabs);

    loadKeys();

    return true;
}

void DatabaseItem::onWheelClick(TreeItem::ParentView& treeView, QWeakPointer<QTabWidget> tabs)
{
    Q_UNUSED(treeView);
    Q_UNUSED(tabs);
}

QSharedPointer<QMenu> DatabaseItem::getContextMenu(TreeItem::ParentView& treeView, QWeakPointer<QTabWidget> tabs)
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

        QRegExp filter;
        QString separator(m_operations->getNamespaceSeparator());        

        QFuture<QList<QSharedPointer<TreeItem>>> keysLoadingResult =
                QtConcurrent::run(&m_keysRenderer,
                                  &KeysTreeRenderer::renderKeys,
                                  m_operations, rawKeys, filter, separator);

        m_keysLoadingWatcher.setFuture(keysLoadingResult);

    });
}

void DatabaseItem::onKeysRendered()
{
    m_keys = m_keysLoadingWatcher.result();
    emit keysLoaded();
}

QList<QSharedPointer<TreeItem> >
DatabaseItem::KeysTreeRenderer::renderKeys(QSharedPointer<Operations> operations,
                                           Operations::RawKeysList keys,
                                           QRegExp filter,
                                           QString namespaceSeparator)
{
    //init
    keys.sort();
    QList<QSharedPointer<TreeItem>> result;

    //render
    for (QString rawKey : keys) {

        //if filter enabled - skip keys
        if (!filter.isEmpty() && !rawKey.contains(filter)) {
            continue;
        }

        renderNamaspacedKey(QSharedPointer<NamespaceItem>(),
                            rawKey, rawKey, operations,
                            namespaceSeparator, result);
    }

    return result;
}

void DatabaseItem::KeysTreeRenderer::renderNamaspacedKey(QSharedPointer<NamespaceItem> currItem,
                                                         const QString &notProcessedKeyPart,
                                                         const QString &fullKey,
                                                         QSharedPointer<Operations> m_operations,
                                                         const QString& m_namespaceSeparator,
                                                         QList<QSharedPointer<TreeItem>>& m_result)
{
    if (!notProcessedKeyPart.contains(m_namespaceSeparator)) {

        QSharedPointer<KeyItem> newKey((new KeyItem(fullKey, m_operations, currItem.data())));

        if (currItem.isNull()) m_result.push_back(newKey);
        else currItem->append(newKey);

        return;
    }

    int indexOfNaspaceSeparator = notProcessedKeyPart.indexOf(m_namespaceSeparator);

    QString firstNamespaceName = notProcessedKeyPart.mid(0, indexOfNaspaceSeparator);

    QSharedPointer<NamespaceItem> namespaceItem;
    int size = (currItem.isNull())? m_result.size() : currItem->childCount();

    for (int i=0; i < size; ++i)
    {
        QSharedPointer<TreeItem> child = (currItem.isNull())? m_result[i] : currItem->child(i);

        if (child->getDisplayName() == firstNamespaceName
                && typeid(NamespaceItem)==typeid(*child)) {

                namespaceItem =  qSharedPointerCast<NamespaceItem>(child);// QSharedPointer<NamespaceItem>(dynamic_cast<NamespaceItem*>(child.data()));
                break;
        }
    }


    if (namespaceItem.isNull()) {
        namespaceItem = QSharedPointer<NamespaceItem>(new NamespaceItem(firstNamespaceName, m_operations, currItem.data()));

        if (currItem.isNull()) m_result.push_back(namespaceItem);
        else currItem->append(namespaceItem);
    }

    renderNamaspacedKey(namespaceItem,
                        notProcessedKeyPart.mid(indexOfNaspaceSeparator+1),
                        fullKey, m_operations, m_namespaceSeparator, m_result);
}
