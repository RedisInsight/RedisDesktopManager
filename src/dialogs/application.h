#pragma  once

#include <QMainWindow>
#include <QMenu>
#include <QSharedPointer>
#include "ui_main.h"

class RedisConnectionsManager;
class Updater;

class MainWin : public QMainWindow
{
    Q_OBJECT

public:
    QSharedPointer<RedisConnectionsManager> connections;

    MainWin(QWidget *parent = 0); 
private:    
    Ui::demoClass ui;                

    QSharedPointer<Updater> updater;    
    QSharedPointer<QMenu> connectionsMenu;

    void initFormButtons();
    void initConnectionsTreeView();    
    void initContextMenus();    
    void initUpdater();
    void initFilter();
    void initSystemConsole();

private slots:
    void OnAddConnectionClick();
    void OnNewUpdateAvailable(QString &);        
    void OnImportConnectionsClick();
    void OnExportConnectionsClick();
    void OnSetFilter();
    void OnClearFilter();   
    void OnConsoleStateChanged();
    void showQuickStartDialog();

};
