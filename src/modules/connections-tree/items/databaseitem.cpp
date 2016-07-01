#include "databaseitem.h"
#include "namespaceitem.h"
#include "keyitem.h"
#include "connections-tree/iconproxy.h"
#include <typeinfo>
#include <functional>
#include <algorithm>
#include <QDebug>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include "connections-tree/utils.h"

using namespace ConnectionsTree;

DatabaseItem::DatabaseItem(unsigned int index, int keysCount,
                           QSharedPointer<Operations> operations,
                           QWeakPointer<TreeItem> parent)
    : m_index(index),
      m_keysCount(keysCount),
      m_locked(false),
      m_operations(operations),
      m_keys(new DatabaseKeys()),
      m_parent(parent),
      m_parentView(nullptr)
{    
    QObject::connect(&m_keysLoadingWatcher, SIGNAL(finished()), this, SLOT(onKeysRendered()));
}

DatabaseItem::~DatabaseItem()
{
    if (m_operations) m_operations->notifyDbWasUnloaded(m_index);
}

QString DatabaseItem::getDisplayName() const
{
    if (m_keys->isEmpty()) {
        return QString("db%1 (%2)").arg(m_index).arg(m_keysCount);
    } else {                
        QString filter =  m_filter.isEmpty()? "" : QString("[filter: %1]").arg(m_filter.pattern());

        return QString("db%1 %2 (%3/%4)").arg(m_index).arg(filter).arg(m_rawKeys.size()).arg(m_keysCount);
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

uint DatabaseItem::childCount(bool) const
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

bool DatabaseItem::onClick(ParentView& view)
{    
    m_parentView = &view;
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
    m_parentView = &treeView;
    QSharedPointer<QMenu> menu(new QMenu());
    std::function<void()> newKeyItemCallback = [this, &treeView]()
    {
        m_operations->openNewKeyDialog(m_index, [this, &treeView]()
        {
            confirmAction(treeView.getParentWidget(),
                          tr("Key was added. Do you want to reload keys in the selected database?"),
                          [this]() { reload(); m_keysCount++; }, tr("Key was added"));
        });
    };
    menu->addAction(createMenuAction(":/images/add.png", "Add new key",
                                     menu.data(), this, newKeyItemCallback,
#ifdef Q_OS_MACX
 QKeySequence("Meta+N")
#else
 QKeySequence("Ctrl+N")
#endif
    ));
    menu->addSeparator();

    if (m_filter.isEmpty()) {
        menu->addAction(createMenuAction(":/images/filter.png", "Filter keys", menu.data(), this,
                                         [this, &treeView]()
        {
            QString text = QInputDialog::getText(treeView.getParentWidget(),
                                                 tr("Filter keys:"),
                                                 tr("Filter regex:"));
            if (!text.isEmpty()) filterKeys(QRegExp(text));
        },
#ifdef Q_OS_MACX
    QKeySequence("Meta+F")
#else
    QKeySequence("Ctrl+F")
#endif
        ));
    } else {
        menu->addAction(createMenuAction(":/images/clear.png", "Reset keys filter", menu.data(), this,
                                         [this]() { resetFilter();},
        QKeySequence("Esc")));
    }
    menu->addSeparator();

    menu->addAction(createMenuAction(":/images/refreshdb.png", "Reload", menu.data(), this,
                                      [this] { this->reload(); },
#ifdef Q_OS_MACX
    QKeySequence("Meta+R")
#else
    QKeySequence("Ctrl+R")
#endif
    ));
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

    m_operations->getDatabaseKeys(m_index, [this](const RedisClient::Connection::RawKeysList& rawKeys, const QString& err) {
        if (!err.isEmpty()) {
            m_locked = false;
            emit error(err);
            emit updateIcon(m_index);

            if (m_parentView)
                QMessageBox::warning(m_parentView->getParentWidget(), tr("Keys error"), err);

            return;
        }
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
    m_rawKeys.clear();
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

void DatabaseItem::renderRawKeys(const RedisClient::Connection::RawKeysList &rawKeys)
{
    qDebug() << "Render keys: " << rawKeys.size();

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
                                           RedisClient::Connection::RawKeysList keys,
                                           QRegExp filter,
                                           QString namespaceSeparator,
                                           QSharedPointer<DatabaseItem> parent)
{
    //init
    QElapsedTimer timer;
    timer.start();    
    std::sort(keys.begin(), keys.end());
    qDebug() << "Keys sorted in: " << timer.elapsed() << " ms";
    QSharedPointer<QList<QSharedPointer<TreeItem>>> result(new QList<QSharedPointer<TreeItem>>());
    QSharedPointer<QHash<QString, QSharedPointer<NamespaceItem>>> rootNamespaces(
                new QHash<QString, QSharedPointer<NamespaceItem>>());

    //render
    timer.restart();
    for (QByteArray rawKey : keys) {

        //if filter enabled - skip keys
        if (!filter.isEmpty()) {
            QString key = QString::fromUtf8(rawKey); // UTF filtering
            if (!key.contains(filter))
                continue;
        }

        renderNamaspacedKey(QSharedPointer<NamespaceItem>(),
                            rawKey, rawKey, operations,
                            namespaceSeparator, result, parent,
                            rootNamespaces);
    }
    qDebug() << "Tree builded in: " << timer.elapsed() << " ms";
    return result;
}

void DatabaseItem::KeysTreeRenderer::renderNamaspacedKey(
        QSharedPointer<NamespaceItem> currItem,
        const QByteArray &notProcessedKeyPart,
        const QByteArray &fullKey,
        QSharedPointer<Operations> m_operations,
        const QString& m_namespaceSeparator,
        QSharedPointer<DatabaseKeys> m_result,
        QSharedPointer<DatabaseItem> db,
        QSharedPointer<QHash<QString, QSharedPointer<NamespaceItem> > > m_rootNamespaces)
{
    QWeakPointer<TreeItem> currentParent = (currItem.isNull())? db.staticCast<TreeItem>().toWeakRef() :
                                                                currItem.staticCast<TreeItem>().toWeakRef();

    int indexOfNaspaceSeparator = (m_namespaceSeparator.isEmpty())?
                -1 : notProcessedKeyPart.indexOf(m_namespaceSeparator);

    if (indexOfNaspaceSeparator == -1) {
        QSharedPointer<KeyItem> newKey(
                    (new KeyItem(fullKey, db->getIndex(), m_operations, currentParent))
                    );

        if (currItem.isNull()) m_result->push_back(newKey);
        else currItem->append(newKey);
        return;
    }

    QString firstNamespaceName = notProcessedKeyPart.mid(0, indexOfNaspaceSeparator);
    QSharedPointer<NamespaceItem> namespaceItem;    

    if (currItem.isNull() && m_rootNamespaces->contains(firstNamespaceName)) {
        namespaceItem = (*m_rootNamespaces)[firstNamespaceName];
    } else if (!currItem.isNull()) {
        namespaceItem = currItem->findChildNamespace(firstNamespaceName);
    }

    if (namespaceItem.isNull()) {
        namespaceItem = QSharedPointer<NamespaceItem>(
                    new NamespaceItem(firstNamespaceName, m_operations, currentParent));

        if (currItem.isNull()) {
            m_result->push_back(namespaceItem);
            m_rootNamespaces->insert(namespaceItem->getName(), namespaceItem);
        }
        else currItem->append(namespaceItem);
    }

    renderNamaspacedKey(namespaceItem, notProcessedKeyPart.mid(indexOfNaspaceSeparator+m_namespaceSeparator.length()),
                        fullKey, m_operations, m_namespaceSeparator, m_result, db, m_rootNamespaces);
}
