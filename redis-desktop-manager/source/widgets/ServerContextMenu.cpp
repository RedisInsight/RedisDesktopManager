#include "ServerContextMenu.h"

ServerContextMenu::ServerContextMenu(QWidget * parent)
{
    addAction(QIcon(":/images/terminal.png"), "Console", parent, SLOT(OnConsoleOpen()));
    addSeparator();
    //menu->addAction(QIcon(":/images/serverinfo.png"), "Server info", parent, SLOT(OnServerInfoOpen()));
    addAction(QIcon(":/images/refreshdb.png"), "Reload", parent, SLOT(OnReloadServerInTree()));
    addAction(QIcon(":/images/redisIcon_offline.png"), "Disconnect", parent, SLOT(OnDisconnectFromServer()));
    addSeparator();
    addAction(QIcon(":/images/editdb.png"), "Edit", parent, SLOT(OnEditConnection()));
    addAction(QIcon(":/images/delete.png"), "Delete", parent, SLOT(OnRemoveConnectionFromTree()));
}
