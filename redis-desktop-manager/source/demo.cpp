#include <QMenu>
#include <QtNetwork>
#include <QFileDialog>
#include <QStatusBar>

#include "demo.h"
#include "connection.h"
#include "RedisServerItem.h"
#include "RedisServerDbItem.h"
#include "RedisKeyItem.h"
#include "valueViewTab.h"
#include "Updater.h"
#include "serverInfoViewTab.h"
#include "consoleTab.h"

MainWin::MainWin(QWidget *parent)
	: QMainWindow(parent), treeViewUILocked(false)
{
	ui.setupUi(this);

	initConnectionsTreeView();
	initServerMenu();
	initConnectionsMenu();
	initFormButtons();	
	initTabs();	
	initUpdater();
	initFilter();

	qRegisterMetaType<RedisConnectionAbstract::RedisDatabases>("RedisConnectionAbstract::RedisDatabases");
	qRegisterMetaType<Command>("Command");
}

MainWin::~MainWin()
{	
	delete connections;
	delete updater;
}

void MainWin::initConnectionsTreeView()
{
	//connection manager
	connections = new RedisConnectionsManager(getConfigPath("connections.xml"), this);

	ui.serversTreeView->setModel(connections);
	ui.serversTreeView->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	ui.serversTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui.serversTreeView->header()->setStretchLastSection(false);
	ui.serversTreeView->setUniformRowHeights(true);

	connect(ui.serversTreeView, SIGNAL(clicked(const QModelIndex&)), 
			this, SLOT(OnConnectionTreeClick(const QModelIndex&)));

	//setup context menu
	ui.serversTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui.serversTreeView, SIGNAL(customContextMenuRequested(const QPoint &)),
			this, SLOT(OnTreeViewContextMenu(const QPoint &)));
}

void MainWin::initServerMenu()
{
	serverMenu = new QMenu();
	serverMenu->addAction(QIcon(":/images/terminal.png"), "Console", this, SLOT(OnConsoleOpen()));
	serverMenu->addSeparator();
	//menu->addAction(QIcon(":/images/serverinfo.png"), "Server info", this, SLOT(OnServerInfoOpen()));
	serverMenu->addAction(QIcon(":/images/refreshdb.png"), "Reload", this, SLOT(OnReloadServerInTree()));
	serverMenu->addAction(QIcon(":/images/redisIcon_offline.png"), "Disconnect", this, SLOT(OnDisconnectFromServer()));
	serverMenu->addSeparator();
	serverMenu->addAction(QIcon(":/images/editdb.png"), "Edit", this, SLOT(OnEditConnection()));
	serverMenu->addAction(QIcon(":/images/delete.png"), "Delete", this, SLOT(OnRemoveConnectionFromTree()));
}

void MainWin::initConnectionsMenu()
{
	connectionsMenu = new QMenu();
	connectionsMenu->addAction(QIcon(":/images/import.png"), "Import Connections", this, SLOT(OnImportConnectionsClick()));
	connectionsMenu->addAction(QIcon(":/images/export.png"), "Export Connections", this, SLOT(OnExportConnectionsClick()));
	connectionsMenu->addSeparator();	

	ui.pbImportConnections->setMenu(connectionsMenu);
}

void MainWin::initFormButtons()
{
	connect(ui.pbAddServer, SIGNAL(clicked()), SLOT(OnAddConnectionClick()));	
	connect(ui.pbImportConnections, SIGNAL(clicked()), SLOT(OnImportConnectionsClick()));
}

void MainWin::initTabs()
{
	connect(ui.tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(OnTabClose(int)));

    #ifndef Q_OS_DARWIN
	//hide close button for first tab
    // on Mac Os this code crash application to segfault
    ui.tabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->setFixedWidth(0);

    #endif
}

void MainWin::initUpdater()
{
	//set current version
	ui.applicationInfoLabel->setText(
		ui.applicationInfoLabel->text().replace("%VERSION%", QApplication::applicationVersion())
		);

	updater = new Updater();
	connect(updater, SIGNAL(updateUrlRetrived(QString &)), this, SLOT(OnNewUpdateAvailable(QString &)));
}

void MainWin::initFilter()
{
	connect(ui.pbFindFilter, SIGNAL(clicked()), SLOT(OnSetFilter()));
	connect(ui.pbClearFilter, SIGNAL(clicked()), SLOT(OnClearFilter()));
}

QString MainWin::getConfigPath(const QString& configFile)
{
	/*
	 * Check current directory
	 */
	QFile testConfig(configFile);	
	QFileInfo checkPermissions(configFile);

	if (!testConfig.exists() && testConfig.open(QIODevice::WriteOnly))
		testConfig.close();

	if (checkPermissions.isWritable()) {
		return configFile;
	}

	/*
	 * Check home user directory
	 */
	QString fullHomePath = QString("%1/%2").arg(QDir::homePath()).arg(configFile);
	testConfig.setFileName(fullHomePath);	
	checkPermissions.setFile(fullHomePath);

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
	connection * connectionDialog = new connection(this);
	connectionDialog->exec();
	delete connectionDialog;
}

void MainWin::OnConnectionTreeClick(const QModelIndex & index)
{
	if (treeViewUILocked) 
		return;	

	QStandardItem * item = connections->itemFromIndex(index);	

	int type = item->type();

	switch (type)
	{
		case RedisServerItem::TYPE:
			{			
				RedisServerItem * server = (RedisServerItem *)item;
				server->runDatabaseLoading();									
			}
			break;

		case RedisServerDbItem::TYPE:
			{
				performanceTimer.start();
				RedisServerDbItem * db = (RedisServerDbItem *)item;
				connections->blockSignals(true);
				db->loadKeys();				
			}			
			break;

		case RedisKeyItem::TYPE:	
			{
				RedisKeyItem * key = (RedisKeyItem *)item;
				QWidget * viewTab = new ValueTab(key);
				
				QString keyFullName = key->getFullText();
				addTab(keyFullName, viewTab);
			}
			break;
	}
}

void MainWin::OnTabClose(int index)
{
	QWidget * w = ui.tabWidget->widget(index);

	ui.tabWidget->removeTab(index);

	delete w;
}

int MainWin::getTabIndex(QString& name)
{
	for (int i = 0; i < ui.tabWidget->count(); ++i)
	{
		if (name == ui.tabWidget->tabText(i)) {
			return i;							
		}
	}

	return -1;
}

void MainWin::addTab(QString& tabName, QWidget* tab, QString icon)
{
	//find opened tab with same key		
	int currIndexOnTab = getTabIndex(tabName);

	if (currIndexOnTab > -1) { // tab exist - close old tab
		OnTabClose(currIndexOnTab);
	}
		
	int currIndex;

	if (icon.isEmpty()) {
		currIndex = ui.tabWidget->addTab(tab, tabName);
	} else {
		currIndex = ui.tabWidget->addTab(tab, QIcon(icon), tabName);
	}

	ui.tabWidget->setCurrentIndex(currIndex);
}

void MainWin::OnTreeViewContextMenu(const QPoint &point)
{
	QStandardItem *item = connections->itemFromIndex(
		ui.serversTreeView->indexAt(point)
		);	

	if (!item)	return;

	int type = item->type();

	if (type == RedisServerItem::TYPE) {
		serverMenu->exec(QCursor::pos());
	}
}

void MainWin::OnReloadServerInTree()
{
	QStandardItem * item = getSelectedItemInConnectionsTree();	

	if (item == nullptr || item->type() != RedisServerItem::TYPE) 
		return;	

	RedisServerItem * server = (RedisServerItem *) item;
	server->reload();
}

void MainWin::OnDisconnectFromServer()
{
	QStandardItem * item = getSelectedItemInConnectionsTree();	

	if (item == nullptr || item->type() != RedisServerItem::TYPE) 
		return;	

	RedisServerItem * server = (RedisServerItem *) item;
	server->unload();
}

void MainWin::OnRemoveConnectionFromTree()
{
	QStandardItem * item = getSelectedItemInConnectionsTree();	

	if (item == nullptr || item->type() != RedisServerItem::TYPE) 
		return;	

	QMessageBox::StandardButton reply;

	reply = QMessageBox::question(this, "Confirm action", "Do you really want delete connection?",
		QMessageBox::Yes|QMessageBox::No);

	if (reply == QMessageBox::Yes) {

		RedisServerItem * server = (RedisServerItem *) item;

		connections->RemoveConnection(server);
	}
}

void MainWin::OnEditConnection()
{
	QStandardItem * item = getSelectedItemInConnectionsTree();	

	if (item == nullptr || item->type() != RedisServerItem::TYPE) 
		return;	

	RedisServerItem * server = (RedisServerItem *) item;

	connection * connectionDialog = new connection(this, server);
	connectionDialog->exec();
	delete connectionDialog;

	server->unload();
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
		QMessageBox::warning(this, "Can't import connections", "Select valid file for import");
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
		QMessageBox::warning(this, "Can't export connections", "Select valid file name for export");
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

	connections->setFilter(filter);

	ui.leKeySearchPattern->setStyleSheet("border: 1px solid green; background-color: #FFFF99;");

}

void MainWin::OnClearFilter()
{
	connections->resetFilter();
	ui.leKeySearchPattern->setStyleSheet("");
}

void MainWin::OnServerInfoOpen()
{
	QStandardItem * item = getSelectedItemInConnectionsTree();	

	if (item == nullptr || item->type() != RedisServerItem::TYPE) 
		return;	

	RedisServerItem * server = (RedisServerItem *) item;

	QStringList info = server->getInfo();

	if (info.isEmpty()) 
		return;

	serverInfoViewTab * tab = new serverInfoViewTab(server->text(), info);
	QString serverName = QString("Info: %1").arg(server->text());
	addTab(serverName, tab, ":/images/serverinfo.png");	
}

void MainWin::OnConsoleOpen()
{
	QStandardItem * item = getSelectedItemInConnectionsTree();	

	if (item == nullptr || item->type() != RedisServerItem::TYPE) 
		return;	

	RedisServerItem * server = (RedisServerItem *) item;
    RedisConnectionConfig config = server->getConnection()->getConfig();

    consoleTab * tab = new consoleTab(config);

	QString serverName = server->text();

	addTab(serverName, tab, ":/images/terminal.png");
}

QStandardItem * MainWin::getSelectedItemInConnectionsTree()
{
	QModelIndexList selected = ui.serversTreeView
									->selectionModel()
									->selectedIndexes();

	if (selected.size() < 1) 
		return nullptr;

	QModelIndex index = selected.at(0);

	if (index.isValid()) {			
		QStandardItem * item = connections->itemFromIndex(index);	

		return item;
	}

	return nullptr;
}

void MainWin::OnError(QString msg)
{
	QMessageBox::warning(this, "Error", msg);
}

void MainWin::OnUIUnlock()
{
	treeViewUILocked = false;
	connections->blockSignals(false);	
	ui.serversTreeView->doItemsLayout();

	statusBar()->showMessage(QString("Keys loaded in: %1 ms").arg(performanceTimer.elapsed()));
	performanceTimer.invalidate();
}
