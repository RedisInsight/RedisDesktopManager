#include <QApplication>
#include <QStyleFactory>
#include <QDir>

#include "CrashHandler.h"
#include "version.h"
#include "application.h"

int main(int argc, char *argv[])
{       
    QApplication a(argc, argv);

    CrashHandler::instance()->Init(QDir::homePath());

    QApplication::setApplicationName("Redis Desktop Manager");
    QApplication::setApplicationVersion(RDM_VERSION);    
    QApplication::setOrganizationDomain("redisdesktop.com");

    MainWin w;
    w.show();
    return a.exec();
}

