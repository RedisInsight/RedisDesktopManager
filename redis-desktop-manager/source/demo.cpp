#include "demo.h"
#include <QMenu>
#include <QFileDialog>
#include <QStatusBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QDialog>
#include <QMovie>

#include "connection.h"
#include "RedisServerItem.h"
#include "RedisServerDbItem.h"
#include "RedisKeyItem.h"
#include "RedisConnectionsManager.h"
#include "RedisConnection.h"
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
	initKeyMenu();
	initConnectionsMenu();
	initFormButtons();	
	initTabs();	
	initUpdater();
	initFilter();

	qRegisterMetaType<RedisConnectionAbstract::RedisDatabases>("RedisConnectionAbstract::RedisDatabases");
	qRegisterMetaType<Command>("Command");	
	qRegisterMetaType<Response>("Response");	
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

	connect(ui.serversTreeView, SIGNAL(clicked(const QModelIndex&)), 
			this, SLOT(OnConnectionTreeClick(const QModelIndex&)));
	connect(ui.serversTreeView, SIGNAL(wheelClicked(const QModelIndex&)), 
		this, SLOT(OnConnectionTreeWheelClick(const QModelIndex&)));

	//setup context menu	
	connect(ui.serversTreeView, SIGNAL(customContextMenuRequested(const QPoint &)),
			this, SLOT(OnTreeViewContextMenu(const QPoint &)));
}

void MainWin::initServerMenu()
{
	// TODO: move to custom QMenu class
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

void MainWin::initKeyMenu()
{
	// TODO: move to custom QMenu class
	keyMenu = new QMenu();
	keyMenu->addAction("Open key value in new tab", this, SLOT(OnKeyOpenInNewTab()));
}

void MainWin::initConnectionsMenu()
{
	// TODO: move to custom QMenu class
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
    // on Mac Os this code crash application
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
	connectionDialog->setWindowState(Qt::WindowActive);
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
				
				ui.serversTreeView->setExpanded(index, true);
			}
			break;

		case RedisServerDbItem::TYPE:
			{
				performanceTimer.start();
				RedisServerDbItem * db = (RedisServerDbItem *)item;
				connections->blockSignals(true);
				statusBar()->showMessage(QString("Loading keys ..."));
				db->loadKeys();				

				ui.serversTreeView->setExpanded(index, true);
			}			
			break;

		case RedisKeyItem::TYPE:	
			openKeyTab((RedisKeyItem *)item);	
			break;
	}
}

void MainWin::openKeyTab(RedisKeyItem * key, bool inNewTab)
{
	QWidget * viewTab = new ValueTab(key);

	connect(viewTab, SIGNAL(keyDeleted(QWidget *, RedisKeyItem *)), 
		this, SLOT(OnKeyDeleted(QWidget *, RedisKeyItem *)));

	connect(viewTab, SIGNAL(error(const QString &)), 
		this, SLOT(OnError(const QString &)));

	QString keyFullName = key->getTabLabelText();

	if (inNewTab) {
		addTab(keyFullName, viewTab, QString(), true);
	} else {
		addTab(keyFullName, viewTab);
	}
}

void MainWin::OnConnectionTreeWheelClick(const QModelIndex & index)
{
	QStandardItem * item = connections->itemFromIndex(index);	

	if (item->type() == RedisKeyItem::TYPE) {
		openKeyTab((RedisKeyItem *)item, true);
	}
}

void MainWin::OnTabClose(int index)
{
	ValueTab * w = qobject_cast<ValueTab *> (ui.tabWidget->widget(index));

	ui.tabWidget->removeTab(index);

	w->close();
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

void MainWin::closeCurrentTabWithValue()
{
	int currIndex = ui.tabWidget->currentIndex();

	if (currIndex == -1) 
		return;
	
	QWidget * w = ui.tabWidget->widget(currIndex);

	if (w->objectName() == "valueTabReady") {
		OnTabClose(currIndex);
	}
}

void MainWin::closeAllServerTabs(RedisServerItem * server)
{
	unsigned int tabsCount = ui.tabWidget->count();

	QString title;

	for (int tabIndex = 0; tabIndex < tabsCount; tabIndex++)
	{
		title = ui.tabWidget->widget(tabIndex)->windowTitle();

		//if (title.contains))

	}
}

void MainWin::addTab(QString& tabName, QWidget* tab, QString icon, bool forceOpenInNewTab)
{		
	int currIndex;

	if (!forceOpenInNewTab) {
		closeCurrentTabWithValue();
	}

	if (icon.isEmpty()) {
		currIndex = ui.tabWidget->addTab(tab, tabName);
	} else {
		currIndex = ui.tabWidget->addTab(tab, QIcon(icon), tabName);
	}

	ui.tabWidget->setCurrentIndex(currIndex);
}


// todo: move responsibility to ConnectionTreeView
// ConnectionTreeView::setItemsContextMenu( QHash( int => QMenu ) )
// and this method will be internal

void MainWin::OnTreeViewContextMenu(const QPoint &point)
{
	if (point.isNull()) return;

	QStandardItem *item = connections->itemFromIndex(
		ui.serversTreeView->indexAt(point)
		);	

	QPoint currentPoint = QCursor::pos(); 

	if (!item || currentPoint.isNull())	return;

	int type = item->type();

	if (type == RedisServerItem::TYPE) {

		if (((RedisServerItem*)item)->isLocked()) {
			QMessageBox::warning(ui.serversTreeView, "Warning", "Connecting to server. Please Keep patience.");
			return;
		}

		serverMenu->exec(currentPoint);
	} else if (type == RedisKeyItem::TYPE) {
		keyMenu->exec(currentPoint);
	}
}

void MainWin::OnReloadServerInTree()
{
	QStandardItem * item = ui.serversTreeView->getSelectedItem(RedisServerItem::TYPE);

	if (item == nullptr) 
		return;	

	RedisServerItem * server = dynamic_cast<RedisServerItem *>(item);
	server->reload();
}

void MainWin::OnDisconnectFromServer()
{
	QStandardItem * item = ui.serversTreeView->getSelectedItem(RedisServerItem::TYPE);	

	if (item == nullptr) 
		return;	

	RedisServerItem * server = (RedisServerItem *) item;
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

		RedisServerItem * server = (RedisServerItem *) item;

		connections->RemoveConnection(server);
	}
}

void MainWin::OnEditConnection()
{
	QStandardItem * item = ui.serversTreeView->getSelectedItem(RedisServerItem::TYPE);	

	if (item == nullptr) 
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
	QStandardItem * item = ui.serversTreeView->getSelectedItem(RedisServerItem::TYPE);	

	if (item == nullptr) 
		return;	

	RedisServerItem * server = (RedisServerItem *) item;
    RedisConnectionConfig config = server->getConnection()->getConfig();

    consoleTab * tab = new consoleTab(config);

	QString serverName = server->text();

	addTab(serverName, tab, ":/images/terminal.png");
}

void MainWin::OnKeyOpenInNewTab()
{
	QStandardItem * item = ui.serversTreeView->getSelectedItem();	

	if (item == nullptr || item->type() != RedisKeyItem::TYPE) 
		return;	

	openKeyTab((RedisKeyItem *)item, true);
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

void MainWin::OnStatusMessage(QString message)
{
	statusBar()->showMessage(message);
}


// TODO: move responsibility to ValueTab + RedisServerDbItem
void MainWin::OnKeyDeleted(QWidget * tab, RedisKeyItem * key)
{
	if (tab == nullptr || key == nullptr) 
		return;

	int widgetsCount = ui.tabWidget->count();
	int widgetIndex = -1;

	for (int currentWidget = 0; currentWidget < widgetsCount; currentWidget++)
	{
		if (tab == ui.tabWidget->widget(currentWidget)) {
			widgetIndex = currentWidget;
			break;
		}
	}	

	if (widgetIndex == -1) {
		return;
	}

	OnTabClose(widgetIndex);	
}
