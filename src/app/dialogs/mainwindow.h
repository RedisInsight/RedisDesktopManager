#pragma  once

#include <QMainWindow>
#include <QMenu>
#include <QSharedPointer>
#include "ui_main.h"
#include "modules/redisclient/connectionconfig.h"

class ConnectionsManager;
class Updater;

namespace ValueEditor {
    class ViewModel;
}

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
    QSharedPointer<ValueEditor::ViewModel> m_keyValues;

    void initFormButtons();
    void initConnectionsTreeView();    
    void initContextMenus();    
    void initUpdater();    
    void initSystemConsole();
    void initValuesView();

    void openConnectionDialog(RedisClient::ConnectionConfig config = RedisClient::ConnectionConfig());

private slots:
    void OnAddConnectionClick();
    void OnEditConnectionClick(RedisClient::ConnectionConfig config);
    void OnNewUpdateAvailable(QString &);        
    void OnImportConnectionsClick();
    void OnExportConnectionsClick();    
    void OnConsoleStateChanged();
    void showQuickStartDialog();

};
