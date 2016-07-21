#include "serveritem.h"
#include <QAction>
#include <QMenu>
#include <functional>
#include <QDebug>
#include <algorithm>
#include <QMessageBox>

#include "connections-tree/iconproxy.h"
#include "connections-tree/utils.h"
#include "connections-tree/model.h"
#include "databaseitem.h"


using namespace ConnectionsTree;

ServerItem::ServerItem(const QString& name, QSharedPointer<Operations> operations, Model &model)
    : TreeItem(model),
      m_name(name),
      m_locked(false),      
      m_row(0),
      m_operations(operations)
{
}

ServerItem::~ServerItem()
{
}

QString ServerItem::getDisplayName() const
{
    return m_name;
}

bool ServerItem::onClick(TreeItem::ParentView& view)
{
    if (isDatabaseListLoaded())
        return false;

    try {
        load();
    } catch (...) {
        QMessageBox::warning(view.getParentWidget(), tr("Server error"),
                             tr("Cannot load databases list."));
        m_locked = false;
    }

    return isDatabaseListLoaded();
}

QIcon ServerItem::getIcon() const
{
    if (m_locked)    return IconProxy::instance()->get(":/images/wait.png");
    if (isDatabaseListLoaded()) return IconProxy::instance()->get(":/images/redisIcon.png");
    return IconProxy::instance()->get(":/images/redisIcon_offline.png");
}

QList<QSharedPointer<TreeItem> > ServerItem::getAllChilds() const
{
    return m_databases;
}

uint ServerItem::childCount(bool) const
{
    return static_cast<uint>(m_databases.size());
}

QSharedPointer<TreeItem> ServerItem::child(uint row) const
{
    if (row < m_databases.size()) {
        return m_databases.at(row);
    }

    return QSharedPointer<TreeItem>();
}

QWeakPointer<TreeItem> ServerItem::parent() const
{
    return QWeakPointer<TreeItem>();
}

int ServerItem::row() const
{
    return m_row;
}

void ServerItem::setRow(int r)
{
    m_row = r;
}

QSharedPointer<QMenu> ServerItem::getContextMenu(TreeItem::ParentView& treeView)
{
    QSharedPointer<QMenu> menu(new QMenu());    
    menu->addAction(createMenuAction(":/images/terminal.png", "Console", menu.data(), this,
                                     [this]() { m_operations->openConsoleTab();},
#ifdef Q_OS_MACX
    QKeySequence("Meta+T")
#else
    QKeySequence("Ctrl+T")
#endif
    ));
    menu->addSeparator();    

    menu->addAction(createMenuAction(":/images/refreshdb.png", "Reload", menu.data(), this,
                    [this] { this->reload(); },
#ifdef Q_OS_MACX
    QKeySequence("Meta+R")
#else
    QKeySequence("Ctrl+R")
#endif
    ));

    menu->addAction(createMenuAction(":/images/redisIcon_offline.png", "Disconnect", menu.data(), this,
                                     [this] { unload(); }, QKeySequence(QKeySequence::StandardKey::Close)));

    menu->addSeparator();   
    menu->addAction(createMenuAction(":/images/editdb.png", "Edit", menu.data(), this,
                    [this, &treeView]
    {
        confirmAction(treeView.getParentWidget(),
                      tr("All value and console tabs related to this "
                      "connection will be closed. Do you want to continue?"),
                      [this]()
        {
            unload();
            emit editActionRequested();
        });
    },
#ifdef Q_OS_MACX
    QKeySequence("Meta+E")
#else
    QKeySequence("Ctrl+E")
#endif
    ));

    //delete action    
    menu->addAction(createMenuAction(":/images/delete.png", "Delete", menu.data(), this,
                                     [this, &treeView]
    {
         confirmAction(treeView.getParentWidget(),
                       tr("Do you really want delete connection?"),
                       [this]()
         {
             unload();
             emit deleteActionRequested();
         });
     }, QKeySequence(QKeySequence::StandardKey::Delete)));

    return menu;
}

bool ServerItem::isLocked() const
{
    return m_locked;
}

bool ServerItem::isEnabled() const
{
    return true;
}

bool ServerItem::isDatabaseListLoaded() const
{
    return m_locked == false && m_databases.size() > 0;
}

void ServerItem::load()
{
    m_locked = true;
    emit m_model.itemChanged(m_self);  

    std::function<void(RedisClient::DatabaseList)> callback = [this](RedisClient::DatabaseList databases) {

        if (databases.size() == 0)
        {
            m_locked = false;
            return;
        }

        RedisClient::DatabaseList::const_iterator db = databases.constBegin();
        while (db != databases.constEnd()) {
            QSharedPointer<TreeItem> database((new DatabaseItem(db.key(), db.value(), m_operations, m_self, m_model)));
            m_databases.push_back(database);
            ++db;            
        }

        emit m_model.itemChildsLoaded(m_self);

        m_locked = false;        
        emit m_model.itemChanged(m_self);
    };

    try {
        m_operations->getDatabases(callback);
    } catch (const ConnectionsTree::Operations::Exception& e) {
        m_locked = false;
        emit m_model.error("Cannot load databases:\n\n" + QString(e.what()));
    }
}

void ServerItem::unload()
{
    if (!isDatabaseListLoaded())
        return;

    m_locked = true;

    emit m_model.itemChildsUnloaded(m_self);

    m_operations->disconnect();
    m_databases.clear();        

    m_locked = false;
}

void ServerItem::reload()
{
    unload();
    load();
}

void ServerItem::setName(const QString& name)
{
    m_name = name;
}

void ServerItem::setWeakPointer(QWeakPointer<ServerItem> self)
{
    m_self = self;
}
