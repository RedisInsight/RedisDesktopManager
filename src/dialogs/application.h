#pragma  once

#include <QMainWindow>
#include <QMenu>
#include <QSharedPointer>
#include "ui_main.h"

class ConnectionsManager;
class Updater;

class MainWin : public QMainWindow
{
    Q_OBJECT

public:
    QSharedPointer<ConnectionsManager> connections;

    MainWin(QWidget *parent = 0); 
private:    
    Ui::demoClass ui;                

    QSharedPointer<Updater> updater;    
    QSharedPointer<QMenu> connectionsMenu;

    void initFormButtons();
    void initConnectionsTreeView();    
    void initContextMenus();    
    void initUpdater();    
    void initSystemConsole();

private slots:
    void OnAddConnectionClick();
    void OnNewUpdateAvailable(QString &);        
    void OnImportConnectionsClick();
    void OnExportConnectionsClick();    
    void OnConsoleStateChanged();
    void showQuickStartDialog();

};
