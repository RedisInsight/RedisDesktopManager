#ifndef DEMO_H
#define DEMO_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFrame>

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
	bool loadingInProgress;
	Ui::demoClass ui;		
	Updater * updater;

	void loadKeyTab(RedisKeyItem *);
	void addTab(QString&, QWidget*);

	/** @return >=0 if exist **/
	int getTabIndex(QString&);

    QString getConfigPath(const QString&);

	void initFormButtons();
	void initConnectionsTreeView();
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
		void OnNewUpdateAvailable(QString &);
		void OnRemoveConnectionFromTree();
		void OnEditConnection();
		void OnImportConnectionsClick();
		void OnSetFilter();
		void OnClearFilter();
		void OnServerInfoOpen();
		void OnConsoleOpen();
};

#endif // DEMO_H
