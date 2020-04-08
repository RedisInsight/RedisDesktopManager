#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>
#include <QScreen>

#if defined(Q_OS_WIN) | defined(Q_OS_LINUX)
#include <QProcess>
#define RELAUNCH_CODE 1001
#endif

#ifdef CRASHPAD_INTEGRATION
#include "crashpad/handler.h"
#endif

#ifdef LINUX_SIGNALS
#include <sigwatch.h>
#endif

#include "app/app.h"

int main(int argc, char *argv[])
{             
    int returnCode = 0;

#ifdef CRASHPAD_INTEGRATION
    QFileInfo appPath(QString::fromLocal8Bit(argv[0]));
    QString appDir(appPath.absoluteDir().path());
    startCrashpad(appDir);
#endif

#if defined(Q_OS_LINUX)
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

#if defined(Q_OS_WIN)
    if (QGuiApplication::primaryScreen() && QGuiApplication::primaryScreen()->availableSize().width() <= 1920
            && QGuiApplication::primaryScreen()->devicePixelRatio() > 1
            && !QGuiApplication::testAttribute(Qt::AA_DisableHighDpiScaling)) {
        QGuiApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    } else {
        QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    }
#endif

    Application a(argc, argv);

#ifdef LINUX_SIGNALS
    UnixSignalWatcher sigwatch;
    sigwatch.watchForSignal(SIGINT);
    sigwatch.watchForSignal(SIGTERM);
    QObject::connect(&sigwatch, SIGNAL(unixSignal(int)), &a, SLOT(quit()));
#endif
    a.initModels();
    a.initQml();
    returnCode = a.exec();

#if defined(Q_OS_WIN) | defined(Q_OS_LINUX)
    if (returnCode == RELAUNCH_CODE) {
        QProcess::startDetached(a.arguments()[0], a.arguments());
        returnCode = 0;
    }
#endif

    return returnCode;
}

