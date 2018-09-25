#pragma once
#include <QApplication>
#include <QFontDatabase>
#include <QMenu>
#include <QQmlApplicationEngine>
#include <QSharedPointer>

#ifndef RDM_VERSION
#include "../version.h"
#endif

class QmlUtils;
class Events;
class ConnectionsManager;
class Updater;
class LogHandler;
class TabViewModel;
namespace ValueEditor {
class TabsModel;
}
namespace ValueEditor {
class FormattersManager;
}
namespace BulkOperations {
class Manager;
}
namespace Console {
class AutocompleteModel;
}

class Application : public QApplication {
  Q_OBJECT

 public:
  Application(int &argc, char **argv);
  void initModels();
  void initQml();

 private:
  void initAppInfo();
  void initAppFonts();
  void initProxySettings();
  void registerQmlTypes();
  void registerQmlRootObjects();
  void initUpdater();
  void installTranslator();
  void processCmdArgs();

 private slots:
  void OnNewUpdateAvailable(QString &url);

 private:
  QQmlApplicationEngine m_engine;
  QSharedPointer<QmlUtils> m_qmlUtils;
  QSharedPointer<Events> m_events;
  QSharedPointer<ConnectionsManager> m_connections;
  QSharedPointer<Updater> m_updater;
  QSharedPointer<ValueEditor::TabsModel> m_keyValues;
  QSharedPointer<ValueEditor::FormattersManager> m_formattersManager;
  QSharedPointer<BulkOperations::Manager> m_bulkOperations;
  QSharedPointer<TabViewModel> m_consoleModel;
  QSharedPointer<TabViewModel> m_serverStatsModel;
  QSharedPointer<Console::AutocompleteModel> m_consoleAutocompleteModel;
  LogHandler *m_logger;
  QString m_settingsDir;
  QString m_renderingBackend;
};
