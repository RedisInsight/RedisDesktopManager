#ifndef DEMO_H
#define DEMO_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFrame>

#include "ui_demo.h"
#include "RedisConnection.h"
#include "RedisConnectionsManager.h"
#include "RedisConnectionsModel.h"
#include "KeysFilter.h"

class RedisKeyItem;
class Updater;

class Main : public QMainWindow
{
	Q_OBJECT

public:
	RedisConnectionsManager * connections;

	Main(QWidget *parent = 0);
	~Main();	
private:
	bool loadingInProgress;
	Ui::demoClass ui;		
	Updater * updater;
	KeysFilter * filterModel;

	void loadKeyTab(RedisKeyItem *);
    QString getConfigPath(const QString&);

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
};

#endif // DEMO_H
