#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>

#include "app/app.h"
#include "modules/crashhandler/crashhandler.h"

int main(int argc, char *argv[])
{           
#ifdef Q_OS_WIN
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    #ifndef QT_DEBUG
    QFileInfo appPath(QString::fromLocal8Bit(argv[0]));
    QString appDir(appPath.absoluteDir().path());
    QString crashReporterPath = QString("%1/crashreporter").arg(appDir.isEmpty() ? "." : appDir);
    CrashHandler::instance()->Init(QDir::homePath(), appDir, crashReporterPath);
    #endif

    Application a(argc, argv);
    a.initModels();
    a.initQml();
    return a.exec();
}

