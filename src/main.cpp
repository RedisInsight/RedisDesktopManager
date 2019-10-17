#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>
#include <QScreen>

#ifdef CRASHPAD_INTEGRATION
#include "crashpad/handler.h"
#endif

#ifdef LINUX_SIGNALS
#include <sigwatch.h>
#endif

#include "app/app.h"

#define RESTART_CODE 1000

int main(int argc, char *argv[])
{             
    int returnCode = 0;

#ifdef CRASHPAD_INTEGRATION
    QFileInfo appPath(QString::fromLocal8Bit(argv[0]));
    QString appDir(appPath.absoluteDir().path());
    startCrashpad(appDir);
#endif

    bool scalingSetup = false;

#if defined(Q_OS_LINUX)
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    do
    {
        Application a(argc, argv);

#if defined(Q_OS_WIN)
        if (!scalingSetup) {
            if (QGuiApplication::primaryScreen() && QGuiApplication::primaryScreen()->availableSize().width() <= 1920
                    && QGuiApplication::primaryScreen()->devicePixelRatio() > 1
                    && !QGuiApplication::testAttribute(Qt::AA_DisableHighDpiScaling)) {
                QGuiApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
            } else {
                QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
            }
            returnCode = RESTART_CODE;
            scalingSetup = true;
            continue;
        }
#endif

#ifdef LINUX_SIGNALS
        UnixSignalWatcher sigwatch;
        sigwatch.watchForSignal(SIGINT);
        sigwatch.watchForSignal(SIGTERM);
        QObject::connect(&sigwatch, SIGNAL(unixSignal(int)), &a, SLOT(quit()));
#endif
        a.initModels();
        a.initQml();
        returnCode = a.exec();
    } while(returnCode == RESTART_CODE);

    return returnCode;
}

