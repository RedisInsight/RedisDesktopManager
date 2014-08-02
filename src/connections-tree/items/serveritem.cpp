#include "serveritem.h"
#include "connections-tree/iconproxy.h"
#include "databaseitem.h"
#include "core/connectionexception.h"
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <functional>
#include <QCollator>
#include <QDebug>
#include <algorithm>
#include "console/consoletab.h"
#include "connections-tree/tabwidget.h"
#include "widgets/BaseTab.h"

using namespace ConnectionsTree;

ServerItem::ServerItem(const QString& name, QSharedPointer<Operations> operations, const Model& model)
    : m_name(name),
      m_locked(false),
      m_databaseListLoaded(false),
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

bool ServerItem::onClick(TreeItem::ParentView& view, TabWidget&)
{
    if (m_databaseListLoaded)
        return false;

    try {
        load();
    } catch (RedisClient::ConnectionExeption& exception) {
        QMessageBox::warning(view.getParentWidget(), "Server error", exception.what());
        m_locked = false;
    }

    return true;
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

QSharedPointer<TreeItem> ServerItem::child(int row) const
{
    if (0 <= row && row < m_databases.size()) {
        return m_databases.at(row);
    }

    return QSharedPointer<TreeItem>();
}

const TreeItem *ServerItem::parent() const {return nullptr; }

QSharedPointer<QMenu> ServerItem::getContextMenu(TreeItem::ParentView& treeView, TabWidget& tabs)
{
    QSharedPointer<QMenu> menu(new QMenu());

    //console action
    QAction* console = new QAction(QIcon(":/images/terminal.png"), "Console", menu.data());
    QObject::connect(console, &QAction::triggered, this, [this, &treeView, &tabs]() {

        qDebug() << "console action";

        QSharedPointer<Console::ConsoleTab> tab = QSharedPointer<Console::ConsoleTab>(
                    new Console::ConsoleTab(m_operations->getConsoleOperations()));

        tabs.addTab(tab.staticCast<BaseTab>());
    });
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
    QObject::connect(edit, &QAction::triggered, this, [this] { unload(); emit editActionRequested(); });
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

bool ServerItem::isDatabaseListLoaded() const
{
    return m_databaseListLoaded;
}

void ServerItem::load()
{
    m_locked = true;

    std::function<void(Operations::DatabaseList)> callback = [this](Operations::DatabaseList databases) {

        if (databases.size() == 0)
        {
            m_locked = false;
            return;
        }

        Operations::DatabaseList::const_iterator db = databases.constBegin();
        unsigned int index = 0;

        while (db != databases.constEnd()) {

            QSharedPointer<TreeItem> database((new DatabaseItem(db.key(), index, db.value(), m_operations, this)));

            QObject::connect(dynamic_cast<QObject*>(database.data()), SIGNAL(keysLoaded(unsigned int)),
                             this, SIGNAL(keysLoadedInDatabase(unsigned int)));

            m_databases.push_back(database);
            ++db;
            ++index;
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

    m_operations->getDatabases(callback);
}

void ServerItem::unload()
{
    if (!m_databaseListLoaded)
        return;

    m_locked = true;

    m_databases.clear();
    m_databaseListLoaded = false;

    m_operations->disconnect();

    m_locked = false;
}

void ServerItem::reload()
{
    unload();
    load();
}




