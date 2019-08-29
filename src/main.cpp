#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>

#ifdef CRASHPAD_INTEGRATION
#include "crashpad/handler.h"
#endif

#include "app/app.h"

#define RESTART_CODE 1000

int main(int argc, char *argv[])
{           
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif    
    int returnCode = 0;

#ifdef CRASHPAD_INTEGRATION
    QFileInfo appPath(QString::fromLocal8Bit(argv[0]));
    QString appDir(appPath.absoluteDir().path());
    startCrashpad(appDir);
#endif

    do
    {
        Application a(argc, argv);
        a.initModels();
        a.initQml();
        returnCode = a.exec();
    } while(returnCode == RESTART_CODE);

    return returnCode;
}

