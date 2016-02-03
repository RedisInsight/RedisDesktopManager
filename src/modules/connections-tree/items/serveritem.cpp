#include "serveritem.h"
#include <QAction>
#include <QMenu>
#include <functional>
#include <QDebug>
#include <algorithm>
#include <QMessageBox>

#include"connections-tree/utils.h"
#include "databaseitem.h"

using namespace ConnectionsTree;

ServerItem::ServerItem(const QString& name, QSharedPointer<Operations> operations,
                       const Model& model)
    : m_name(name),
      m_locked(false),
      m_databaseListLoaded(false),
      m_row(0),
      m_operations(operations),
      m_model(model)
{

    m_eventHandlers.insert("click", [this]() {
        if (m_databaseListLoaded)
            return;

        load();
    });

    m_eventHandlers.insert("console", [this]() {
        m_operations->openConsoleTab();
    });

    m_eventHandlers.insert("reload", [this]() {
        reload();
    });

    m_eventHandlers.insert("unload", [this]() {
        unload();
    });

    m_eventHandlers.insert("edit", [this]() {
        confirmAction(nullptr, tr("All value and console tabs related to this"
                                  "connection will be closed. Do you want to continue?"), [this]()
         {
             unload();
             emit editActionRequested();
         });
    });

    m_eventHandlers.insert("delete", [this]() {
        confirmAction(nullptr, tr("Do you really want delete connection?"), [this]()
         {
             unload();
             emit deleteActionRequested();
         });
    });
}

ServerItem::~ServerItem()
{
}

QString ServerItem::getDisplayName() const
{
    return m_name;
}

QString ServerItem::getIconUrl() const
{
    if (m_locked)    return QString("qrc:/images/wait.svg");
    if (m_databaseListLoaded) return QString("qrc:/images/server.svg");
    return QString("qrc:/images/server.svg"); //offline
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
        while (db != databases.constEnd()) {
            QSharedPointer<TreeItem> database((new DatabaseItem(db->first, db->second, m_operations, m_self)));

            QObject::connect(dynamic_cast<QObject*>(database.data()), SIGNAL(keysLoaded(unsigned int)),
                             this, SIGNAL(keysLoadedInDatabase(unsigned int)));
            QObject::connect(dynamic_cast<QObject*>(database.data()), SIGNAL(unloadStarted(unsigned int)),
                             this, SIGNAL(unloadStartedInDatabase(unsigned int)));
            QObject::connect(dynamic_cast<QObject*>(database.data()), SIGNAL(updateIcon(unsigned int)),
                             this, SIGNAL(updateDbIcon(unsigned int)));
            QObject::connect(database.dynamicCast<DatabaseItem>().data(), &DatabaseItem::error,
                             this, &ServerItem::error);

            m_databases.push_back(database);
            ++db;            
        }
        m_locked = false;
        m_databaseListLoaded = true;

        emit databaseListLoaded();
    };

    try {
        m_operations->getDatabases(callback);
    } catch (const ConnectionsTree::Operations::Exception& e) {
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

void ServerItem::edit()
{
    unload();
    emit editActionRequested();
}

void ServerItem::remove()
{
    unload();
    emit deleteActionRequested();
}

void ServerItem::openConsole()
{
    m_operations->openConsoleTab();
}

void ServerItem::setName(const QString& name)
{
    m_name = name;
}

void ServerItem::setWeakPointer(QWeakPointer<ServerItem> self)
{
    m_self = self;
}
