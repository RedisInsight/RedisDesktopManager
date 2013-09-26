#include <QMenu>
#include <QtNetwork>

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

Main::Main(QWidget *parent)
	: QMainWindow(parent), loadingInProgress(false)
{
	ui.setupUi(this);

	// connect slots to signals
	connect(ui.pbAddServer, SIGNAL(clicked()), SLOT(OnAddConnectionClick()));	

	connect(ui.serversTreeView, SIGNAL(clicked(const QModelIndex&)), 
			this, SLOT(OnConnectionTreeClick(const QModelIndex&)));

	//tabWidget setup
	connect(ui.tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(OnTabClose(int)));
	ui.tabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->hide(); //hide tabButton for first tab

	//connection manager
	connections = new RedisConnectionsManager(getConfigPath("connections.xml"));
	ui.serversTreeView->setModel(connections);
	ui.serversTreeView->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	ui.serversTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui.serversTreeView->header()->setStretchLastSection(false);

	//setup context menu
	ui.serversTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui.serversTreeView, SIGNAL(customContextMenuRequested(const QPoint &)),
			this, SLOT(OnTreeViewContextMenu(const QPoint &)));

	//Updater
	//set current version
	ui.currentVersionLabel->setText(
		ui.currentVersionLabel->text() + QApplication::applicationVersion()
		);

	updater = new Updater();
	connect(updater, SIGNAL(updateUrlRetrived(QString &)),
		this, SLOT(OnNewUpdateAvailable(QString &)));

}

Main::~Main()
{	
	delete connections;
	delete updater;
}

QString Main::getConfigPath(const QString& configFile)
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


void Main::OnAddConnectionClick()
{
	connection * connectionDialog = new connection(this);
	connectionDialog->setWindowFlags(Qt::Tool);		
	connectionDialog->setModal(true);
	connectionDialog->exec();
	delete connectionDialog;
}

void Main::OnConnectionTreeClick(const QModelIndex & index)
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
				server->loadDatabases();
				loadingInProgress = false;
				
				//TODO : create new tab with server info
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

void Main::OnTabClose(int index)
{
	QWidget * w = ui.tabWidget->widget(index);

	ui.tabWidget->removeTab(index);

	delete w;
}

void Main::loadKeyTab(RedisKeyItem * key)
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

		//find opened tab with same key
		bool isTabReplaced = false;
		int insertedTabIndex = 0;
		QString keyFullName = key->getFullText();

		for (int i = 0; i < ui.tabWidget->count(); ++i)
		{
			if (keyFullName == ui.tabWidget->tabText(i)) {
				OnTabClose(i);
				ui.tabWidget->insertTab(i, viewTab, keyFullName);
				insertedTabIndex = i;				
				break;
			}
		}

		if (!isTabReplaced) {
			insertedTabIndex = ui.tabWidget->addTab(viewTab, keyFullName);			
		}

		ui.tabWidget->setCurrentIndex(insertedTabIndex);
	}

	key->setNormalIcon();
}

void Main::OnTreeViewContextMenu(const QPoint &point)
{
	QStandardItem *item = connections->itemFromIndex(ui.serversTreeView->indexAt(point));	

	if (!item)	return;

	int type = item->type();

	if (type == RedisServerItem::TYPE) {
		QMenu *menu = new QMenu();
		menu->addAction("Reload", this, SLOT(OnReloadServerInTree()));
		menu->exec(QCursor::pos());
	}
}

void Main::OnReloadServerInTree()
{
	QModelIndexList selected = ui.serversTreeView->selectionModel()->selectedIndexes();

	if (selected.size() == 0) 
		return;

	for (auto index : selected) {
		QStandardItem * item = connections->itemFromIndex(index);	

		if (item->type() == RedisServerItem::TYPE) {
			RedisServerItem * server = (RedisServerItem *) item;

			server->reload();
		}
	}
}

void Main::OnNewUpdateAvailable(QString &url)
{
	ui.newUpdateAvailableLabel->setText(QString("<div style=\"font-size: 13px;\">New update available: %1</div>").arg(url));
}


