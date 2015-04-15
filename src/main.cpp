#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QSettings>
#include <QFontDatabase>

#ifndef RDM_VERSION
#include "version.h"
#endif
#include "modules/crashhandler/crashhandler.h"
#include "app/dialogs/mainwindow.h"

int main(int argc, char *argv[])
{       
    QApplication a(argc, argv);

#ifndef QT_DEBUG
    CrashHandler::instance()->Init(QDir::homePath());
#endif

    QApplication::setApplicationName("Redis Desktop Manager");
    QApplication::setApplicationVersion(QString(RDM_VERSION));
    QApplication::setOrganizationDomain("redisdesktop.com");
    
    QFontDatabase::addApplicationFont("://fonts/OpenSans-Regular.ttf");
    QFont defaultFont("OpenSans", 10);
    QApplication::setFont(defaultFont);

    MainWin w;
    w.show();
    return a.exec();
}

