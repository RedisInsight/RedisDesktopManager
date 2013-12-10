#ifndef DEMO_H
#define DEMO_H

#include <QtWidgets/QMainWindow>
#include <QElapsedTimer>
#include "ui_demo.h"

class RedisKeyItem;
class RedisServerItem;
class RedisConnectionsManager;
class Updater;

class MainWin : public QMainWindow
{
	Q_OBJECT

public:
	RedisConnectionsManager * connections;

	MainWin(QWidget *parent = 0);
	~MainWin();	
private:
	bool treeViewUILocked;
	Ui::demoClass ui;		
	Updater * updater;
	QElapsedTimer performanceTimer;
	QMenu * serverMenu;
	QMenu * keyMenu;
	QMenu * connectionsMenu;

	// TODO: move to custom tabWidget class
	void addTab(QString&, QWidget*, QString icon = QString(), bool forceOpenInNewTab = false);
	// todo: move to custom tabWidget class
	void closeCurrentTabWithValue();
	// todo: move to custom tabWidget class
	void closeAllServerTabs(RedisServerItem *);
	// todo: move to custom tabWidget class
	void openKeyTab(RedisKeyItem * key, bool inNewTab = false);

	// todo: move to custom tabWidget class
	/** @return >=0 if exist **/
	int getTabIndex(QString&);


	// todo: move to custom Settings class
    QString getConfigPath(const QString&);

	void initFormButtons();
	void initConnectionsTreeView();


	//todo combine in one method - initContextMenus()
	void initServerMenu();
	void initKeyMenu();
	void initConnectionsMenu();
	// end todo


	void initTabs();
	void initUpdater();
	void initFilter();

	private slots:
		void OnAddConnectionClick();
		void OnConnectionTreeClick(const QModelIndex & index);
		void OnConnectionTreeWheelClick(const QModelIndex & index);
		void OnTabClose(int index);
		void OnTreeViewContextMenu(const QPoint &);
		void OnReloadServerInTree();
		void OnDisconnectFromServer();
		void OnNewUpdateAvailable(QString &);
		void OnRemoveConnectionFromTree();
		void OnEditConnection();
		void OnImportConnectionsClick();
		void OnExportConnectionsClick();
		void OnSetFilter();
		void OnClearFilter();
		void OnServerInfoOpen();
		void OnConsoleOpen();
		void OnError(QString);
		void OnUIUnlock();
		void OnStatusMessage(QString);
		void OnKeyOpenInNewTab();
		void OnKeyDeleted(QWidget * tab, RedisKeyItem * key);
};

#endif // DEMO_H
