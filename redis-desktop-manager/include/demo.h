#ifndef DEMO_H
#define DEMO_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFrame>
#include <QMovie>

#include "ui_demo.h"
#include "RedisConnection.h"
#include "RedisConnectionsManager.h"

class RedisKeyItem;
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
	QMenu * connectionsMenu;

	void addTab(QString&, QWidget*, QString icon = QString());

	/** @return >=0 if exist **/
	int getTabIndex(QString&);

    QString getConfigPath(const QString&);

	void initFormButtons();
	void initConnectionsTreeView();
	void initServerMenu();
	void initConnectionsMenu();
	void initTabs();
	void initUpdater();
	void initFilter();

	QStandardItem * getSelectedItemInConnectionsTree();

	private slots:
		void OnAddConnectionClick();
		void OnConnectionTreeClick(const QModelIndex & index);
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
};

#endif // DEMO_H
