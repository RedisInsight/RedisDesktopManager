#include "databaseitem.h"

#include <typeinfo>
#include <functional>
#include <algorithm>
#include <QDebug>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QFutureWatcher>

#include "namespaceitem.h"
#include "keyitem.h"
#include "serveritem.h"
#include "connections-tree/iconproxy.h"
#include "connections-tree/utils.h"
#include "connections-tree/model.h"

using namespace ConnectionsTree;

DatabaseItem::DatabaseItem(unsigned int index, int keysCount,
                           QSharedPointer<Operations> operations,
                           QWeakPointer<TreeItem> parent,
                           Model& model)
    : AbstractNamespaceItem(model, parent, operations),
      m_index(index),
      m_keysCount(keysCount),
      m_locked(false),                  
      m_parentView(nullptr)
{        
    m_renderingSettings.nsSeparator = operations->getNamespaceSeparator();
    m_renderingSettings.dbIndex = index;
}

DatabaseItem::~DatabaseItem()
{
    if (m_operations) m_operations->notifyDbWasUnloaded(m_index);
}

QString DatabaseItem::getName() const
{
    return QString();
}

QByteArray DatabaseItem::getFullPath() const
{
    return QByteArray();
}

QString DatabaseItem::getDisplayName() const
{
    if (m_childItems.isEmpty()) {
        return QString("db%1 (%2)").arg(m_index).arg(m_keysCount);
    } else {                
        QString filter =  m_renderingSettings.filter.isEmpty()? "" : QString("[filter: %1]").arg(m_renderingSettings.filter.pattern());

        return QString("db%1 %2 (%3/%4)").arg(m_index).arg(filter).arg(m_rawChilds.size()).arg(m_keysCount);
    }
}

QIcon DatabaseItem::getIcon() const
{
    if (m_locked)    return IconProxy::instance()->get(":/images/wait.png");
    return IconProxy::instance()->get(":/images/db.png");
}

bool DatabaseItem::onClick(ParentView& view)
{    
    m_parentView = &view;
    if (m_childItems.size() == 0) {
        loadKeys();
        return true;
    }
    return false;
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

    if (m_renderingSettings.filter.isEmpty()) {
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
    menu->addSeparator();
    menu->addSeparator();
    menu->addAction(createMenuAction(":/images/delete.png", "Flush database", menu.data(), this,
                                     [this, &treeView]
    {
        confirmAction(treeView.getParentWidget(),
                      tr("Do you really want to remove all keys from this database?"),
                      [this]()
        {
            m_operations->flushDb(m_index, [this](const QString& err)
            {
                unload();
                m_keysCount = 0;
            });
        });
    }));

    return menu;
}

bool DatabaseItem::isLocked() const {return m_locked;}

bool DatabaseItem::isEnabled() const {return true;}

void DatabaseItem::notifyModel()
{
    m_locked = false;
    AbstractNamespaceItem::notifyModel();
}

void DatabaseItem::loadKeys()
{
    if (m_rawChilds.size() > 0) {
        clear(false);
        renderChilds();
        return;
    }

    m_locked = true;
    emit m_model.itemChanged(getSelf());

    m_operations->getDatabaseKeys(m_index, [this](const RedisClient::Connection::RawKeysList& rawKeys, const QString& err) {
        if (!err.isEmpty()) {
            m_locked = false;

            emit m_model.itemChanged(getSelf());
            emit m_model.error(err);

            if (m_parentView)
                QMessageBox::warning(m_parentView->getParentWidget(), tr("Keys error"), err);

            return;
        }
        m_rawChilds = rawKeys;
        renderChilds();
    });
}

int DatabaseItem::getIndex() const
{
    return m_index;
}

void DatabaseItem::unload(bool removeRawKeys)
{
    if (m_childItems.size() == 0)
        return;

    m_locked = true;
    clear(removeRawKeys);

    m_operations->notifyDbWasUnloaded(m_index);

    m_locked = false;
    emit m_model.itemChanged(getSelf());
}

void DatabaseItem::reload()
{
    unload();
    loadKeys();
}

void DatabaseItem::filterKeys(const QRegExp &filter)
{
    m_renderingSettings.filter = filter;
    loadKeys();
}

void DatabaseItem::resetFilter()
{
    m_renderingSettings.filter = QRegExp();
    loadKeys();
}
