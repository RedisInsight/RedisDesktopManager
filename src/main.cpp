#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>

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

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    bool disableAutoScaling = false;


#ifndef DISABLE_SCALING_TEST
    {
        QGuiApplication tmp(argc, argv);
        disableAutoScaling = QGuiApplication::primaryScreen()
                        && QGuiApplication::primaryScreen()->availableSize().width() <= 1920
                        && QGuiApplication::primaryScreen()->devicePixelRatio() == 1;
    }
#endif

#if defined (Q_OS_LINUX) && defined(DISABLE_SCALING_TEST)
    disableAutoScaling = true;
#endif

    if (disableAutoScaling) {
        qDebug() << "Disable auto-scaling";
        QGuiApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    } else {
        qDebug() << "Enable auto-scaling";
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

