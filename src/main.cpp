#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QSettings>

#include "crashhandler/crashhandler.h"
#include "version.h"
#include "dialogs/application.h"

int main(int argc, char *argv[])
{       
    QApplication a(argc, argv);

    CrashHandler::instance()->Init(QDir::homePath());

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

