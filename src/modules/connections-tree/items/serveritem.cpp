#include "serveritem.h"
#include "connections-tree/iconproxy.h"
#include "databaseitem.h"
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <functional>
#include <QDebug>
#include <algorithm>

using namespace ConnectionsTree;

ServerItem::ServerItem(const QString& name, QSharedPointer<Operations> operations, const Model& model)
    : m_name(name),
      m_locked(false),
      m_databaseListLoaded(false),
      m_row(0),
      m_operations(operations),
      m_model(model)
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
    if (m_databaseListLoaded)
        return false;

    try {
        load();
    } catch (...) {
        QMessageBox::warning(
            view.getParentWidget(),
            "Server error",
            "Cannot load databases list."
        );
        m_locked = false;
    }

    return m_databaseListLoaded;
}

QIcon ServerItem::getIcon() const
{
    if (m_locked)    return IconProxy::instance()->get(":/images/wait.png");
    if (m_databaseListLoaded) return IconProxy::instance()->get(":/images/redisIcon.png");
    return IconProxy::instance()->get(":/images/redisIcon_offline.png");
}

QList<QSharedPointer<TreeItem> > ServerItem::getAllChilds() const
{
    return m_databases;
}

uint ServerItem::childCount() const
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

QWeakPointer<TreeItem> ServerItem::parent() const { return QWeakPointer<TreeItem>(); }

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

    //console action
    QAction* console = new QAction(QIcon(":/images/terminal.png"), "Console", menu.data());
    QObject::connect(console, &QAction::triggered, this, [this]() { m_operations->openConsoleTab(); });
    menu->addAction(console);
    menu->addSeparator();    

    //reload action
    QAction* reload = new QAction(QIcon(":/images/refreshdb.png"), "Reload", menu.data());
    QObject::connect(reload, &QAction::triggered, this, [this] { this->reload(); });
    menu->addAction(reload);

     //disconnect action
    QAction* disconnect = new QAction(QIcon(":/images/redisIcon_offline.png"), "Disconnect", menu.data());
    QObject::connect(disconnect, &QAction::triggered, this, [this] { unload(); });
    menu->addAction(disconnect);
    menu->addSeparator();

    //edit action
    QAction* edit = new QAction(QIcon(":/images/editdb.png"), "Edit", menu.data());
    QObject::connect(edit, &QAction::triggered, this, [this, &treeView] {

        QMessageBox::StandardButton reply = QMessageBox::question(
                    treeView.getParentWidget(), "Confirm action",
                    "All value and console tabs related to this "
                    "connection will be closed. Do you want to continue?",
                    QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            unload();
            emit editActionRequested();
        }
    });
    menu->addAction(edit);

    //delete action
    QAction* del = new QAction(QIcon(":/images/delete.png"), "Delete", menu.data());
    QObject::connect(del, &QAction::triggered, this, [this, &treeView] {

        QMessageBox::StandardButton reply = QMessageBox::question(
                    treeView.getParentWidget(), "Confirm action",
                    "Do you really want delete connection?",
                    QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            unload();
            emit deleteActionRequested();
        }
    });
    menu->addAction(del);

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
    return m_databaseListLoaded;
}

void ServerItem::load()
{
    m_locked = true;
    emit updateIcon();

    std::function<void(Operations::DatabaseList)> callback = [this](Operations::DatabaseList databases) {

        if (databases.size() == 0)
        {
            m_locked = false;
            return;
        }

        Operations::DatabaseList::const_iterator db = databases.constBegin();
        int index;
        QString dbIndex;

        while (db != databases.constEnd()) {
            dbIndex = db.key();
            dbIndex = dbIndex.remove(0,2);
            index = dbIndex.toInt();

            QSharedPointer<TreeItem> database((new DatabaseItem(db.key(), index, db.value(), m_operations, m_self)));

            QObject::connect(dynamic_cast<QObject*>(database.data()), SIGNAL(keysLoaded(unsigned int)),
                             this, SIGNAL(keysLoadedInDatabase(unsigned int)));
            QObject::connect(dynamic_cast<QObject*>(database.data()), SIGNAL(unloadStarted(unsigned int)),
                             this, SIGNAL(unloadStartedInDatabase(unsigned int)));
            QObject::connect(dynamic_cast<QObject*>(database.data()), SIGNAL(updateIcon(unsigned int)),
                             this, SIGNAL(updateDbIcon(unsigned int)));

            m_databases.push_back(database);
            ++db;            
        }

        std::sort(m_databases.begin(), m_databases.end(), [](QSharedPointer<TreeItem> left, QSharedPointer<TreeItem> right) {
            QString leftName = left->getDisplayName();
            QString rightName = right->getDisplayName();

            QCollator collator;
            collator.setNumericMode(true);

            return collator.compare(leftName, rightName) == -1? true : false;
        });

        m_locked = false;
        m_databaseListLoaded = true;

        emit databaseListLoaded();
    };

    try {
        m_operations->getDatabases(callback);
    } catch (ConnectionsTree::Operations::Exception& e) {
        m_locked = false;
        emit error("Cannot load databases: " + QString(e.what()));
    }
}

void ServerItem::unload()
{
    if (!m_databaseListLoaded)
        return;

    m_locked = true;

    emit unloadStarted();

    m_databaseListLoaded = false;
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
