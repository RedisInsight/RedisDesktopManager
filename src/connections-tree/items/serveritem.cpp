#include "serveritem.h"
#include "connections-tree/iconproxy.h"
#include "databaseitem.h"
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <functional>

using namespace ConnectionsTree;

ServerItem::ServerItem(QSharedPointer<Operations> operations)
    : m_operations(operations)
{
}

QString ServerItem::getDisplayName() const
{
    return m_name;
}

bool ServerItem::onClick(QWeakPointer<ParentView>, QWeakPointer<QTabWidget>)
{
    load();
    return true;
}

QIcon ServerItem::getIcon() const
{
    if (m_locked)    return IconProxy::instance()->get(":/images/wait.png");
    if (m_connected) return IconProxy::instance()->get(":/images/redisIcon.png");
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
        return QSharedPointer<TreeItem>(m_databases.at(row));
    }

    QSharedPointer<TreeItem>();
}

QSharedPointer<QMenu> ServerItem::getContextMenu(QWeakPointer<TreeItem::ParentView> treeView, QWeakPointer<QTabWidget> tabs)
{
    QSharedPointer<QMenu> menu(new QMenu());

    /*
    //console action
    QAction* console = new QAction(QIcon(":/images/terminal.png"), "Console", menu.data());
    QObject::connect(console, &QAction::triggered, this, [this]() {

//        QStandardItem * item = ui.serversTreeView->getSelectedItem(RedisServerItem::TYPE);

//        if (item == nullptr)
//            return;

//        RedisServerItem * server = dynamic_cast<RedisServerItem *>(item);
//        RedisClient::ConnectionConfig config = server->getConnection()->getConfig();

//        BaseTab * tab = new BaseTab();
//        ConsoleTab * console = new ConsoleTab(config);
//        console->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
//        QBoxLayout * layout = new QBoxLayout(QBoxLayout::LeftToRight, tab);
//        layout->setMargin(0);
//        layout->addWidget(console);
//        tab->setLayout(layout);
//        tab->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
//        console->setParent(tab);

//        QString serverName = server->text();

//        ui.tabWidget->addTab(serverName, tab, ":/images/terminal.png", true);

//        console->setFocus();



    });
    menu->addAction(console);
    menu->addSeparator();

    //reload action
    QAction* reload = new QAction(QIcon(":/images/refreshdb.png"), "Reload", menu.data());
    QObject::connect(reload, &QAction::triggered, this, [this] {




    });
    menu->addAction(reload);

    //disconnect action
    QAction* disconnect = new QAction(QIcon(":/images/redisIcon_offline.png"), "Disconnect", menu.data());
    QObject::connect(disconnect, &QAction::triggered, this, [this] {




    });
    menu->addAction(disconnect);
    menu->addSeparator();

    //edit action
    QAction* edit = new QAction(QIcon(":/images/editdb.png"), "Edit", menu.data());
    QObject::connect(edit, &QAction::triggered, this, [this] {




    });
    menu->addAction(edit);

    //delete action
    QAction* del = new QAction(QIcon(":/images/delete.png"), "Delete", menu.data());
    QObject::connect(del, &QAction::triggered, this, [this, treeView] {

        QMessageBox::StandardButton reply;

        reply = QMessageBox::question(treeView->getParentWidget(), "Confirm action", "Do you really want delete connection?",
            QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            treeView->removeCurrentItem();
        }
    });
    menu->addAction(del);*/

    return menu;
}

bool ServerItem::isLocked() const
{
    return m_locked;
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

        QHash<QString, int>::const_iterator db = databases.constBegin();
        unsigned int index = 0;

        while (db != databases.constEnd()) {

            m_databases.push_back(
                        QSharedPointer<TreeItem>(
                            new DatabaseItem(db.key(), index, db.value(), m_operations)
                            )
                        );
            ++db;
            ++index;
        }

        m_locked = false;
        m_databaseListLoaded = true;

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

//void MainWin::OnRemoveConnectionFromTree()
//{

//}

//void MainWin::OnEditConnection()
//{
//    QStandardItem * item = ui.serversTreeView->getSelectedItem(RedisServerItem::TYPE);

//    if (item == nullptr)
//        return;

//    RedisServerItem * server = dynamic_cast<RedisServerItem *>(item);

//    server->unload();
//    UnlockUi();

//    QScopedPointer<ConnectionWindow> connectionDialog( new ConnectionWindow(this, server) );
//    connectionDialog->setModal(true);
//    connectionDialog->setWindowState(Qt::WindowActive);
//#ifdef Q_OS_LINUX
//    connectionDialog->move(this->width()/2 + connectionDialog->width() / 3, this->height()/2);
//#endif
//    connectionDialog->exec();
//}

//void MainWin::OnServerInfoOpen()
//{
//    QStandardItem * item = ui.serversTreeView->getSelectedItem(RedisServerItem::TYPE);

//    if (item == nullptr)
//        return;

//    RedisServerItem * server = dynamic_cast<RedisServerItem *>(item);

//    QStringList info = server->getInfo();

//    if (info.isEmpty())
//        return;

//    serverInfoViewTab * tab = new serverInfoViewTab(server->text(), info);
//    QString serverName = QString("Info: %1").arg(server->text());
//    ui.tabWidget->addTab(serverName, tab, ":/images/serverinfo.png");
//}

//QStringList RedisServerItem::getInfo()
//{
//    if (!connection->connect()) {
//        setOfflineIcon();
//        return QStringList();
//    }

//    return connection->operations()->getInfo();
//}




