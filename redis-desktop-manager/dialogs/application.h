#pragma  once

#include <QtWidgets/QMainWindow>
#include <QElapsedTimer>
#include <QScopedPointer>
#include "ui_demo.h"

class RedisKeyItem;
class RedisServerItem;
class RedisConnectionsManager;
class Updater;

class MainWin : public QMainWindow
{
    Q_OBJECT

public:
    QSharedPointer<RedisConnectionsManager> connections;

    MainWin(QWidget *parent = 0); 
private:
    bool m_treeViewUILocked;
    Ui::demoClass ui;            
    QElapsedTimer performanceTimer;

    QSharedPointer<Updater> updater;
    QSharedPointer<QMenu> serverMenu;
    QSharedPointer<QMenu> keyMenu;
    QSharedPointer<QMenu> connectionsMenu;

    void initFormButtons();
    void initConnectionsTreeView();    
    void initContextMenus();    
    void initUpdater();
    void initFilter();
    void initSystemConsole();

    void lockUi();
    bool isUiLocked();

private slots:
    void OnAddConnectionClick();
    void OnConnectionTreeClick(const QModelIndex & index);
    void OnConnectionTreeWheelClick(const QModelIndex & index);
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
    void OnLogMessage(QString);

    void OnStatusMessage(QString);
    void OnKeyOpenInNewTab();
    void OnConsoleStateChanged();

public slots:
      void UnlockUi();
};
