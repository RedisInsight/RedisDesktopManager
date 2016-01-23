#pragma once
#include <QApplication>
#include <QSharedPointer>
#include <QQmlApplicationEngine>
#include <QFontDatabase>
#include <QMenu>
#include <googlemp.h>

#ifndef RDM_VERSION
#include "../version.h"
#endif

class QmlUtils;
class ConnectionsManager;
class Updater;
class LogHandler;
namespace ValueEditor { class ViewModel; }
namespace Console { class ViewModel; }

class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int &argc, char **argv);
    void initModels();
    void initQml();

private:
    void initAppInfo();
    void initAppFonts();
    void initAppAnalytics();
    void registerQmlTypes();
    void registerQmlRootObjects();
    void initLog();
    void initConnectionsManager();
    void initUpdater();

private slots:
      void OnNewUpdateAvailable(QString &url);

private:
    QQmlApplicationEngine m_engine;
    QSharedPointer<QmlUtils> m_qmlUtils;
    QSharedPointer<ConnectionsManager> m_connections;
    QSharedPointer<Updater> m_updater;
    QSharedPointer<ValueEditor::ViewModel> m_keyValues;
    QSharedPointer<Console::ViewModel> m_consoleModel;
    LogHandler* m_logger;
};
