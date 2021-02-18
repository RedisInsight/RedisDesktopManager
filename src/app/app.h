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
class KeyFactory;
class TabViewModel;
class QPython;
namespace ValueEditor {
class TabsModel;
}
namespace ValueEditor {
#ifdef ENABLE_EXTERNAL_FORMATTERS
class ExternalFormattersManager;
#endif
class EmbeddedFormattersManager;
}  // namespace ValueEditor
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
  ~Application() override;

  void initModels();
  void initQml();

 private:
  void initAppInfo();
  void initAppFonts();
  void initProxySettings();
  void initPython();

  void registerQmlTypes();
  void registerQmlRootObjects();
  void installTranslator();
  void processCmdArgs();

 private slots:
  void updatePalette();

 private:
  QQmlApplicationEngine m_engine;
  QSharedPointer<QmlUtils> m_qmlUtils;
  QSharedPointer<Events> m_events;
  QSharedPointer<ConnectionsManager> m_connections;
  QSharedPointer<KeyFactory> m_keyFactory;
  QSharedPointer<ValueEditor::TabsModel> m_keyValues;
#ifdef ENABLE_EXTERNAL_FORMATTERS
  QSharedPointer<ValueEditor::ExternalFormattersManager> m_formattersManager;
#endif
  QSharedPointer<ValueEditor::EmbeddedFormattersManager> m_embeddedFormatters;
  QSharedPointer<BulkOperations::Manager> m_bulkOperations;
  QSharedPointer<TabViewModel> m_consoleModel;
  QSharedPointer<TabViewModel> m_serverStatsModel;
  QSharedPointer<Console::AutocompleteModel> m_consoleAutocompleteModel;
  QSharedPointer<QPython> m_python;
  QString m_settingsDir;
  QString m_formattersDir;
  QString m_renderingBackend;
};
