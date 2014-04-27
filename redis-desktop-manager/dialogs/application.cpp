#include "application.h"

#include <QMenu>
#include <QFileDialog>
#include <QStatusBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QDialog>
#include <QMovie>

#include "connection.h"
#include "Redis.h"
#include "valueTab.h"
#include "Updater.h"
#include "serverInfoViewTab.h"
#include "consoleTab.h"
#include "ServerContextMenu.h"

MainWin::MainWin(QWidget *parent)
    : QMainWindow(parent), treeViewUILocked(false)
{
    ui.setupUi(this);

    initConnectionsTreeView();
    initContextMenus();
    initFormButtons();    
    initUpdater();
    initFilter();
    initSystemConsole();
    
    qRegisterMetaType<RedisConnectionAbstract::RedisDatabases>("RedisConnectionAbstract::RedisDatabases");
    qRegisterMetaType<Command>("Command");    
    qRegisterMetaType<Response>("Response");  

    performanceTimer.invalidate();
}

void MainWin::initConnectionsTreeView()
{
    //connection manager
    connections = QSharedPointer<RedisConnectionsManager>(new RedisConnectionsManager(getConfigPath("connections.xml"), this));

    ui.serversTreeView->setModel(connections.data());

    connect(ui.serversTreeView, SIGNAL(clicked(const QModelIndex&)), 
            this, SLOT(OnConnectionTreeClick(const QModelIndex&)));
    connect(ui.serversTreeView, SIGNAL(wheelClicked(const QModelIndex&)), 
        this, SLOT(OnConnectionTreeWheelClick(const QModelIndex&)));

    //setup context menu    
    connect(ui.serversTreeView, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(OnTreeViewContextMenu(const QPoint &)));
}

void MainWin::initContextMenus()
{
    serverMenu = QSharedPointer<ServerContextMenu>(new ServerContextMenu(this));

    // TODO: move to custom QMenu class
    keyMenu = QSharedPointer<QMenu>(new QMenu());
    keyMenu->addAction("Open key value in new tab", this, SLOT(OnKeyOpenInNewTab()));

    // TODO: move to custom QMenu class
    connectionsMenu = QSharedPointer<QMenu>(new QMenu());
    connectionsMenu->addAction(QIcon(":/images/import.png"), "Import Connections", this, SLOT(OnImportConnectionsClick()));
    connectionsMenu->addAction(QIcon(":/images/export.png"), "Export Connections", this, SLOT(OnExportConnectionsClick()));
    connectionsMenu->addSeparator();    

    ui.pbImportConnections->setMenu(connectionsMenu.data());
}

void MainWin::initFormButtons()
{
    connect(ui.pbAddServer, SIGNAL(clicked()), SLOT(OnAddConnectionClick()));    
    connect(ui.pbImportConnections, SIGNAL(clicked()), SLOT(OnImportConnectionsClick()));
}

void MainWin::initUpdater()
{
    //set current version
    ui.applicationInfoLabel->setText(
        ui.applicationInfoLabel->text().replace("%VERSION%", QApplication::applicationVersion())
        );

    updater = QSharedPointer<Updater>(new Updater());
    connect(updater.data(), SIGNAL(updateUrlRetrived(QString &)), this, SLOT(OnNewUpdateAvailable(QString &)));
}

void MainWin::initFilter()
{
    connect(ui.pbFindFilter, SIGNAL(clicked()), SLOT(OnSetFilter()));
    connect(ui.pbClearFilter, SIGNAL(clicked()), SLOT(OnClearFilter()));
}

void MainWin::initSystemConsole()
{
    QPushButton * systemConsoleActivator = new QPushButton( QIcon(":/images/terminal.png"), "System log" );

    connect(systemConsoleActivator, SIGNAL(clicked()), this, SLOT(OnConsoleStateChanged()));

    ui.systemConsole->hide();
    ui.statusBar->addPermanentWidget(systemConsoleActivator);
}

void MainWin::OnConsoleStateChanged()
{
    ui.systemConsole->setVisible(!ui.systemConsole->isVisible());
}

QString MainWin::getConfigPath(const QString& configFile)
{
    /*
     * Check home user directory
     */
    QString fullHomePath = QString("%1/%2").arg(QDir::homePath()).arg(configFile);
    QFile testConfig(fullHomePath);    
    QFileInfo checkPermissions(fullHomePath);

    if (!testConfig.exists() && testConfig.open(QIODevice::WriteOnly))
        testConfig.close();

    if (checkPermissions.isWritable()) {
        return fullHomePath;
    }
    
    QMessageBox::warning(this, 
        "Current directory is not writable",
        "Program can't save connections file to current dir. Please change permissions or restart this program with administrative privileges"
        );
    exit(1);
}

void MainWin::OnAddConnectionClick()
{
    QScopedPointer<ConnectionWindow> connectionDialog(new ConnectionWindow(this));    
    connectionDialog->setWindowState(Qt::WindowActive);
    connectionDialog->exec();    
}

void MainWin::OnConnectionTreeClick(const QModelIndex & index)
{
    if (treeViewUILocked || !index.isValid()) 
        return;    

    QStandardItem * item = connections->itemFromIndex(index);    

    int type = item->type();

    switch (type) {
    case RedisServerItem::TYPE:
    {            
        RedisServerItem * server = dynamic_cast<RedisServerItem *>(item);
        server->runDatabaseLoading();                
        ui.serversTreeView->setExpanded(index, true);
    }
    break;

    case RedisServerDbItem::TYPE:
    {        
        RedisServerDbItem * db = dynamic_cast<RedisServerDbItem *>(item);

        if (db->loadKeys()) {
            performanceTimer.start();
            connections->blockSignals(true);
            statusBar()->showMessage(QString("Loading keys ..."));
            ui.serversTreeView->setExpanded(index, true);
        }                
                     
    }            
    break;

    case RedisKeyItem::TYPE:    

        if (item->isEnabled())
            ui.tabWidget->openKeyTab(dynamic_cast<RedisKeyItem *>(item));    

        break;
    }
}

void MainWin::OnConnectionTreeWheelClick(const QModelIndex & index)
{
    if (!index.isValid())
        return;

    QStandardItem * item = connections->itemFromIndex(index);    

    if (item->type() == RedisKeyItem::TYPE) {
        ui.tabWidget->openKeyTab((RedisKeyItem *)item, true);
    }
}


// todo: move responsibility to ConnectionTreeView
// ConnectionTreeView::setItemsContextMenu( QHash( int => QMenu ) )
// and this method will be internal

void MainWin::OnTreeViewContextMenu(const QPoint &point)
{
    if (point.isNull()) 
        return;

    QStandardItem *item = connections->itemFromIndex(
        ui.serversTreeView->indexAt(point)
        );    

    QPoint currentPoint = QCursor::pos(); 

    if (!item || currentPoint.isNull() || treeViewUILocked)
        return;

    int type = item->type();

    if (type == RedisServerItem::TYPE) {

        if (((RedisServerItem*)item)->isLocked()) {
            QMessageBox::warning(ui.serversTreeView, "Warning", "Performing operations. Please Keep patience.");
            return;
        }

        QAction * action = serverMenu->exec(currentPoint);

        if (action == nullptr)
            return;
            
        if (action->text() == "Reload")
            treeViewUILocked = true;
        
    } else if (type == RedisKeyItem::TYPE) {
        keyMenu->exec(currentPoint);
    }
}

void MainWin::OnReloadServerInTree()
{
    QStandardItem * item = ui.serversTreeView->getSelectedItem(RedisServerItem::TYPE);

    if (item == nullptr) 
        return;    

    try {
        RedisServerItem * server = dynamic_cast<RedisServerItem *>(item);
        server->reload();
    } catch (std::bad_cast &exception) {
        QMessageBox::warning(this, "Error", "Error occurred on reloading connection");
    }
}

void MainWin::OnDisconnectFromServer()
{
    QStandardItem * item = ui.serversTreeView->getSelectedItem(RedisServerItem::TYPE);    

    if (item == nullptr) 
        return;    

    RedisServerItem * server = dynamic_cast<RedisServerItem *>(item);
    server->unload();
}

void MainWin::OnRemoveConnectionFromTree()
{
    QStandardItem * item = ui.serversTreeView->getSelectedItem(RedisServerItem::TYPE);

    if (item == nullptr) 
        return;    

    QMessageBox::StandardButton reply;

    reply = QMessageBox::question(this, "Confirm action", "Do you really want delete connection?",
        QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {

        RedisServerItem * server = dynamic_cast<RedisServerItem *>(item);

        connections->RemoveConnection(server);
        treeViewUILocked = false;
    }
}

void MainWin::OnEditConnection()
{
    QStandardItem * item = ui.serversTreeView->getSelectedItem(RedisServerItem::TYPE);    

    if (item == nullptr) 
        return;    

    RedisServerItem * server = dynamic_cast<RedisServerItem *>(item);
    
    server->unload();
    treeViewUILocked = false;

    QScopedPointer<ConnectionWindow> connectionDialog( new ConnectionWindow(this, server) );
    connectionDialog->exec();    
}

void MainWin::OnNewUpdateAvailable(QString &url)
{
    QMessageBox::information(this, "New update available", 
        QString("Please download new version of Redis Desktop Manager: %1").arg(url));
}

void MainWin::OnImportConnectionsClick()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Import Connections", "", tr("Xml Files (*.xml)"));

    if (fileName.isEmpty()) {
        return;
    }

    if (connections->ImportConnections(fileName)) {
        QMessageBox::information(this, "Connections imported", "Connections imported from connections file");
    } else {
        QMessageBox::warning(this, "Can't import connections", "Select valid file for import");
    }
}

void MainWin::OnExportConnectionsClick()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export Connections to xml", "", tr("Xml Files (*.xml)"));

    if (fileName.isEmpty()) {
        return;
    }

    if (connections->SaveConnectionsConfigToFile(fileName)) {
        QMessageBox::information(this, "Connections exported", "Connections exported in selected file");
    } else {
        QMessageBox::warning(this, "Can't export connections", "Select valid file name for export");
    }
}

void MainWin::OnSetFilter()
{
    QRegExp filter(ui.leKeySearchPattern->text());

    if (filter.isEmpty() || !filter.isValid()) {
        ui.leKeySearchPattern->setStyleSheet("border: 2px dashed red;");
        return;
    }

    performanceTimer.start();

    connections->setFilter(filter);

    ui.leKeySearchPattern->setStyleSheet("border: 1px solid green; background-color: #FFFF99;");
    ui.pbClearFilter->setEnabled(true);
}

void MainWin::OnClearFilter()
{
    performanceTimer.start();
    connections->resetFilter();
    ui.leKeySearchPattern->setStyleSheet("");
    ui.pbClearFilter->setEnabled(false);
}

void MainWin::OnServerInfoOpen()
{
    QStandardItem * item = ui.serversTreeView->getSelectedItem(RedisServerItem::TYPE);    

    if (item == nullptr) 
        return;    

    RedisServerItem * server = dynamic_cast<RedisServerItem *>(item);

    QStringList info = server->getInfo();

    if (info.isEmpty()) 
        return;

    serverInfoViewTab * tab = new serverInfoViewTab(server->text(), info);
    QString serverName = QString("Info: %1").arg(server->text());
    ui.tabWidget->addTab(serverName, tab, ":/images/serverinfo.png");    
}

void MainWin::OnConsoleOpen()
{
    QStandardItem * item = ui.serversTreeView->getSelectedItem(RedisServerItem::TYPE);    

    if (item == nullptr) 
        return;    

    RedisServerItem * server = dynamic_cast<RedisServerItem *>(item);
    RedisConnectionConfig config = server->getConnection()->getConfig();

    BaseTab * tab = new BaseTab();
    ConsoleTab * console = new ConsoleTab(config);
    console->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    QBoxLayout * layout = new QBoxLayout(QBoxLayout::LeftToRight, tab);
    layout->setMargin(0);
    layout->addWidget(console);
    tab->setLayout(layout);
    tab->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    console->setParent(tab);    

    QString serverName = server->text();

    ui.tabWidget->addTab(serverName, tab, ":/images/terminal.png", true);

    console->setFocus();
}

void MainWin::OnKeyOpenInNewTab()
{
    QStandardItem * item = ui.serversTreeView->getSelectedItem();    

    if (item == nullptr || item->type() != RedisKeyItem::TYPE) 
        return;    

    ui.tabWidget->openKeyTab((RedisKeyItem *)item, true);
}

void MainWin::OnError(QString msg)
{
    QMessageBox::warning(this, "Error", msg);
}

void MainWin::OnLogMessage(QString message)
{
    ui.systemConsole->appendPlainText(QString("[%1] %2").arg(QTime::currentTime().toString()).arg(message));
}

void MainWin::OnUIUnlock()
{
    treeViewUILocked = false;

    if (connections->signalsBlocked()) {
        connections->blockSignals(false);    
        ui.serversTreeView->doItemsLayout();
    }

    if (performanceTimer.isValid()) {
        statusBar()->showMessage(QString("Keys loaded in: %1 ms").arg(performanceTimer.elapsed()));
        performanceTimer.invalidate();
    }
}

void MainWin::OnStatusMessage(QString message)
{
    statusBar()->showMessage(message);
}
