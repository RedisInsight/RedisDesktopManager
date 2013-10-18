#include <QMenu>
#include <QtNetwork>
#include <QFileDialog>

#include "demo.h"
#include "connection.h"
#include "RedisServerItem.h"
#include "RedisServerDbItem.h"
#include "RedisKeyItem.h"
#include "stringViewTab.h"
#include "hashViewTab.h"
#include "listViewTab.h"
#include "zsetViewTab.h"
#include "Updater.h"
#include "serverInfoViewTab.h"
#include "consoleTab.h"

MainWin::MainWin(QWidget *parent)
	: QMainWindow(parent), loadingInProgress(false)
{
	ui.setupUi(this);

	initConnectionsTreeView();
	initFormButtons();	
	initTabs();	
	initUpdater();
	initFilter();
}

MainWin::~MainWin()
{	
	delete connections;
	delete updater;
}

void MainWin::initConnectionsTreeView()
{
	//connection manager
	connections = new RedisConnectionsManager(getConfigPath("connections.xml"));	

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
    ui.tabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->hide();

    #endif
}

void MainWin::initUpdater()
{
	//set current version
	ui.currentVersionLabel->setText(
		ui.currentVersionLabel->text() + QApplication::applicationVersion()
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
	if (loadingInProgress) {
		return;
	}

	QStandardItem * item = connections->itemFromIndex(index);

	int type = item->type();
	switch (type)
	{
		case RedisServerItem::TYPE:
			{			
				RedisServerItem * server = (RedisServerItem *)item;
				loadingInProgress = true;
				bool connected = server->loadDatabases();
				connections->updateFilter();		
				loadingInProgress = false;
				
				if (!connected) {
					QMessageBox::warning(this, "Can't connect to server", "Can't connect to server. Check connection settings");
				}				
			}
			break;
		case RedisServerDbItem::TYPE:
			{
				RedisServerDbItem * db = (RedisServerDbItem *)item;
				loadingInProgress = true;
				db->loadKeys();
				loadingInProgress = false;
			}			
			break;

		case RedisKeyItem::TYPE:	
			loadingInProgress = true;
			loadKeyTab((RedisKeyItem *)item);		
			loadingInProgress = false;
			break;

		default:
			break;
	}
}

void MainWin::OnTabClose(int index)
{
	QWidget * w = ui.tabWidget->widget(index);

	ui.tabWidget->removeTab(index);

	delete w;
}

void MainWin::loadKeyTab(RedisKeyItem * key)
{	
	key->setBusyIcon();
	RedisKeyItem::Type type = key->getKeyType();

	QWidget * viewTab = nullptr;

	switch (type)
	{
	case RedisKeyItem::String:		
		viewTab = new stringViewTab(key->text(), key->getValue().toString());					
		break;

	case RedisKeyItem::Hash:		
		viewTab = new hashViewTab(key->text(), key->getValue().toStringList());			
		break;

	case RedisKeyItem::List:		
	case RedisKeyItem::Set:
		viewTab = new listViewTab(key->text(), key->getValue().toStringList());	
		break;

	case RedisKeyItem::ZSet:
		viewTab = new zsetViewTab(key->text(), key->getValue().toStringList());
		break;	
	}

	if (viewTab != nullptr) {
					
		QString keyFullName = key->getFullText();

		addTab(keyFullName, viewTab);
	}

	key->setNormalIcon();
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
		QMenu *menu = new QMenu();
		menu->addAction(QIcon(":/images/terminal.png"), "Console", this, SLOT(OnConsoleOpen()));
		menu->addSeparator();
		menu->addAction(QIcon(":/images/serverinfo.png"), "Server info", this, SLOT(OnServerInfoOpen()));
		menu->addAction(QIcon(":/images/refreshdb.png"), "Reload", this, SLOT(OnReloadServerInTree()));
		menu->addSeparator();
		menu->addAction(QIcon(":/images/editdb.png"), "Edit", this, SLOT(OnEditConnection()));
		menu->addAction(QIcon(":/images/delete.png"), "Delete", this, SLOT(OnRemoveConnectionFromTree()));
		menu->exec(QCursor::pos());
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
	ui.newUpdateAvailableLabel->setText(QString("<div style=\"font-size: 13px;\">New update available: %1</div>").arg(url));
}

void MainWin::OnImportConnectionsClick()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Import Connections", "", tr("Xml Files (*.xml)"));

	if (fileName.isEmpty()) {
		QMessageBox::warning(this, "Can't import connections", "Select valid file for import");
	}

	if (connections->ImportConnections(fileName)) {
		QMessageBox::information(this, "Connections imported", "Connections imported from connections file");
	} else {
		QMessageBox::warning(this, "Can't import connections", "Select valid file for import");
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
	QString serverName = server->text();
	addTab(serverName, tab, ":/images/serverinfo.png");	
}

void MainWin::OnConsoleOpen()
{
	QStandardItem * item = getSelectedItemInConnectionsTree();	

	if (item == nullptr || item->type() != RedisServerItem::TYPE) 
		return;	

	RedisServerItem * server = (RedisServerItem *) item;
	consoleTab * tab = new consoleTab(server->getConnection()->config);


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
