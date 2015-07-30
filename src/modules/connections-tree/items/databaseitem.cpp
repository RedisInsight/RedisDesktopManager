#include "databaseitem.h"
#include "namespaceitem.h"
#include "keyitem.h"
#include "connections-tree/iconproxy.h"
#include <typeinfo>
#include <functional>
#include <QDebug>
#include <QMenu>
#include <QInputDialog>
#include "connections-tree/utils.h"

using namespace ConnectionsTree;

DatabaseItem::DatabaseItem(const QString& displayName,
                           unsigned int index, int keysCount,
                           QSharedPointer<Operations> operations,
                           QWeakPointer<TreeItem> parent)
    : m_name(displayName),
      m_index(index),
      m_keysCount(keysCount),
      m_locked(false),
      m_operations(operations),
      m_keys(new DatabaseKeys()),
      m_parent(parent)
{    
    QObject::connect(&m_keysLoadingWatcher, SIGNAL(finished()), this, SLOT(onKeysRendered()));
}

DatabaseItem::~DatabaseItem()
{
    if (m_operations) m_operations->notifyDbWasUnloaded(m_index);
}

QString DatabaseItem::getDisplayName() const
{
    if (!m_filter.isEmpty()) {
      return QString("%1 (filter: %2)").arg(m_name).arg(m_filter.pattern());
    } else if (m_keys->isEmpty()) {
        return m_name;
    } else {
        return QString("%1 (%2/%3)").arg(m_name).arg(childCount()).arg(m_keysCount);
    }
}

QIcon DatabaseItem::getIcon() const
{
    if (m_locked)    return IconProxy::instance()->get(":/images/wait.png");
    return IconProxy::instance()->get(":/images/db.png");
}

QList<QSharedPointer<TreeItem> > DatabaseItem::getAllChilds() const
{
    return *m_keys;
}

uint DatabaseItem::childCount() const
{
    return m_keys->size();
}

QSharedPointer<TreeItem> DatabaseItem::child(uint row) const
{
    if (row < childCount())
        return m_keys->at(row);

    return QSharedPointer<TreeItem>();
}

QWeakPointer<TreeItem> DatabaseItem::parent() const
{
    return m_parent;
}

bool DatabaseItem::onClick(ParentView&)
{    
    if (m_rawKeys.size() == 0) {
        loadKeys();
        return true;
    }
    return false;
}

void DatabaseItem::onWheelClick(TreeItem::ParentView&)
{    
}

QSharedPointer<QMenu> DatabaseItem::getContextMenu(TreeItem::ParentView& treeView)
{
    QSharedPointer<QMenu> menu(new QMenu());
    menu->addAction(createMenuAction(":/images/add.png", "Add new key", menu.data(), this,
                                     [this]() { m_operations->openNewKeyDialog(m_index); }));
    menu->addSeparator();

    if (m_filter.isEmpty()) {
        menu->addAction(createMenuAction(":/images/filter.png", "Filter keys", menu.data(), this,
                                         [this, &treeView]()
        {
            QString text = QInputDialog::getText(treeView.getParentWidget(),
                                                 tr("Filter keys:"),
                                                 tr("Filter regex:"));
            if (!text.isEmpty()) filterKeys(QRegExp(text));
        }));
    } else {
        menu->addAction(createMenuAction(":/images/clear.png", "Reset keys filter", menu.data(), this,
                                         [this]() { resetFilter();}));
    }
    menu->addSeparator();

    menu->addAction(createMenuAction(":/images/refreshdb.png", "Reload", menu.data(), this,
                                      [this] { this->reload(); }));
    return menu;
}

bool DatabaseItem::isLocked() const {return m_locked;}

bool DatabaseItem::isEnabled() const {return true;}

void DatabaseItem::loadKeys()
{
    if (m_rawKeys.size() > 0) {
        renderRawKeys(m_rawKeys);
        emit keysLoaded(m_index);
        return;
    }

    m_locked = true;
    emit updateIcon(m_index);

    m_operations->getDatabaseKeys(m_index, [this](const Operations::RawKeysList& rawKeys) {
        m_rawKeys = rawKeys;
        renderRawKeys(rawKeys);
    });
}

int DatabaseItem::getIndex() const
{
    return m_index;
}

void DatabaseItem::onKeysRendered()
{
    m_keys = m_keysLoadingWatcher.result();
    m_locked = false;
    emit keysLoaded(m_index);
}

void DatabaseItem::unload()
{
    if (m_keys->size() == 0)
        return;

    m_locked = true;
    emit unloadStarted(m_index);
    m_operations->notifyDbWasUnloaded(m_index);
    m_keys = QSharedPointer<DatabaseKeys>(new DatabaseKeys());
    m_locked = false;
}

void DatabaseItem::reload()
{
    unload();
    loadKeys();
}

void DatabaseItem::filterKeys(const QRegExp &filter)
{
    m_filter = filter;
    loadKeys();
}

void DatabaseItem::resetFilter()
{
    m_filter = QRegExp();
    loadKeys();
}

void DatabaseItem::renderRawKeys(const Operations::RawKeysList &rawKeys)
{
    qDebug() << "Keys: " << rawKeys.size();

    if (rawKeys.size() == 0) {
        m_locked = false;
        return;
    }

    QString separator(m_operations->getNamespaceSeparator());

    QSharedPointer<TreeItem> server = parent().toStrongRef();

    if (!server || !server->child(row())) {
        qDebug() << "Cannot render keys: invalid parent item";
        return;
    }

    QSharedPointer<DatabaseItem> self = server->child(row()).staticCast<DatabaseItem>();

    QFuture<QSharedPointer<QList<QSharedPointer<TreeItem>>>> keysLoadingResult =
            QtConcurrent::run(&KeysTreeRenderer::renderKeys,
                              m_operations, rawKeys, m_filter, separator, self);

    m_keysLoadingWatcher.setFuture(keysLoadingResult);
}

QSharedPointer<DatabaseKeys> DatabaseItem::KeysTreeRenderer::renderKeys(QSharedPointer<Operations> operations,
                                           Operations::RawKeysList keys,
                                           QRegExp filter,
                                           QString namespaceSeparator,
                                           QSharedPointer<DatabaseItem> parent)
{
    //init
    keys.sort();
    QSharedPointer<QList<QSharedPointer<TreeItem>>> result(new QList<QSharedPointer<TreeItem>>());

    //render
    for (QVariant key : keys) {

        QString rawKey = key.toString();

        //if filter enabled - skip keys
        if (!filter.isEmpty() && !rawKey.contains(filter)) {
            continue;
        }

        renderNamaspacedKey(QSharedPointer<NamespaceItem>(),
                            rawKey, rawKey, operations,
                            namespaceSeparator, result, parent);
    }

    return result;
}

void DatabaseItem::KeysTreeRenderer::renderNamaspacedKey(QSharedPointer<NamespaceItem> currItem,
                                                         const QString &notProcessedKeyPart,
                                                         const QString &fullKey,
                                                         QSharedPointer<Operations> m_operations,
                                                         const QString& m_namespaceSeparator,
                                                         QSharedPointer<DatabaseKeys> m_result,
                                                         QSharedPointer<DatabaseItem> db)
{
    QWeakPointer<TreeItem> currentParent = (currItem.isNull())? db.staticCast<TreeItem>().toWeakRef() :
                                                                currItem.staticCast<TreeItem>().toWeakRef();

    if (!notProcessedKeyPart.contains(m_namespaceSeparator) || m_namespaceSeparator.isEmpty()) {

        QSharedPointer<KeyItem> newKey(
                    (new KeyItem(fullKey, db->getIndex(), m_operations, currentParent))
                    );

        if (currItem.isNull()) m_result->push_back(newKey);
        else currItem->append(newKey);

        return;
    }

    int indexOfNaspaceSeparator = notProcessedKeyPart.indexOf(m_namespaceSeparator);

    QString firstNamespaceName = notProcessedKeyPart.mid(0, indexOfNaspaceSeparator);

    QSharedPointer<NamespaceItem> namespaceItem;
    int size = (currItem.isNull())? m_result->size() : currItem->childCount();

    for (int i=0; i < size; ++i)
    {
        QSharedPointer<TreeItem> child = (currItem.isNull())? (*m_result)[i] : currItem->child(i);

        if (child->getDisplayName() == firstNamespaceName
                && typeid(NamespaceItem)==typeid(*child)) {

                namespaceItem =  qSharedPointerCast<NamespaceItem>(child);
                break;
        }
    }

    if (namespaceItem.isNull()) {
        namespaceItem = QSharedPointer<NamespaceItem>(
                    new NamespaceItem(firstNamespaceName, m_operations, currentParent));

        if (currItem.isNull()) m_result->push_back(namespaceItem);
        else currItem->append(namespaceItem);
    }

    renderNamaspacedKey(namespaceItem, notProcessedKeyPart.mid(indexOfNaspaceSeparator+m_namespaceSeparator.length()),
                        fullKey, m_operations, m_namespaceSeparator, m_result, db);
}
