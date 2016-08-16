#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QSettings>
#include <QFontDatabase>
#include <googlemp.h>

#ifndef RDM_VERSION
#include "version.h"
#endif
#include "modules/crashhandler/crashhandler.h"
#include "app/dialogs/mainwindow.h"

int main(int argc, char *argv[])
{       
#ifdef Q_OS_WIN
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication a(argc, argv);

#ifndef QT_DEBUG
    CrashHandler::instance()->Init(QDir::homePath());
#endif

    QApplication::setApplicationName("Redis Desktop Manager");
    QApplication::setApplicationVersion(QString(RDM_VERSION));
    QApplication::setOrganizationDomain("redisdesktop.com");
    QApplication::setOrganizationName("redisdesktop");
    
    QFontDatabase::addApplicationFont("://fonts/OpenSans.ttc");
    QFont defaultFont("OpenSans", 10);
    QApplication::setFont(defaultFont);

    GoogleMP::startSession(QDateTime::currentMSecsSinceEpoch());
    GoogleMP::instance()->reportEvent("rdm:cpp", "app start", "");

    MainWin w;
    w.setWindowTitle(QString("Redis Desktop Manager v.%1").arg(QString(RDM_VERSION)));
    w.show();
    return a.exec();
}

