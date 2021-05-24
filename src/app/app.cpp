#include "app.h"

#include <qpython.h>
#include <pythonlib_loader.h>
#include <qredisclient/redisclient.h>
#include <QMessageBox>
#include <QNetworkProxyFactory>
#include <QQmlContext>
#include <QQuickWindow>
#include <QSettings>
#include <QSysInfo>
#include <QUrl>
#include <QtQml>
#include <QSslSocket>
#include <QtConcurrent>

#if defined(Q_OS_WINDOWS) || defined(Q_OS_LINUX)
#include "darkmode.h"
#include <QStyleFactory>
#endif

#include "common/tabviewmodel.h"
#include "events.h"
#include "models/configmanager.h"
#include "models/connectionconf.h"
#include "models/connectionsmanager.h"
#include "models/key-models/keyfactory.h"
#include "modules/bulk-operations/bulkoperationsmanager.h"
#include "modules/common/sortfilterproxymodel.h"
#include "modules/console/autocompletemodel.h"
#include "modules/console/consolemodel.h"
#include "modules/server-stats/serverstatsmodel.h"
#include "modules/value-editor/embeddedformattersmanager.h"
#ifdef ENABLE_EXTERNAL_FORMATTERS
#include "modules/value-editor/externalformattersmanager.h"
#endif
#include "modules/value-editor/syntaxhighlighter.h"
#include "modules/value-editor/textcharformat.h"
#include "modules/value-editor/tabsmodel.h"
#include "modules/value-editor/valueviewmodel.h"
#include "qmlutils.h"

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv),
      m_engine(this),
      m_qmlUtils(QSharedPointer<QmlUtils>(new QmlUtils())),
      m_events(QSharedPointer<Events>(new Events())) {
  // Init components required for models and qml
  initAppInfo();
  initProxySettings();
  processCmdArgs();
  initAppFonts();

#if defined(Q_OS_WINDOWS) || defined(Q_OS_LINUX)
  if (isDarkThemeEnabled()) {
    setStyle(QStyleFactory::create("Fusion"));
    setPalette(createDarkModePalette());
  }
#endif

  initRedisClient();
  installTranslator();
  initPython();
}

Application::~Application()
{
    m_connections.clear();
}

void Application::initModels() {
  ConfigManager confManager(m_settingsDir);

  QString config = confManager.getApplicationConfigPath("connections.json");

  if (config.isNull()) {
    QMessageBox::critical(
        nullptr,
        QCoreApplication::translate("RDM",
                                    "Settings directory is not writable"),
        QCoreApplication::translate(
            "RDM",
            "RDM can't save connections file to settings directory. "
            "Please change file permissions or restart RDM as "
            "administrator."));

    throw std::runtime_error("invalid connections config");
  }

  m_keyFactory = QSharedPointer<KeyFactory>(new KeyFactory());

  m_keyValues =
      QSharedPointer<ValueEditor::TabsModel>(new ValueEditor::TabsModel(
          m_keyFactory.staticCast<ValueEditor::AbstractKeyFactory>(), m_events));

  connect(m_events.data(), &Events::openValueTab, m_keyValues.data(),
          &ValueEditor::TabsModel::openTab);
  connect(m_events.data(), &Events::newKeyDialog, m_keyFactory.data(),
          &KeyFactory::createNewKeyRequest);
  connect(m_events.data(), &Events::closeDbKeys, m_keyValues.data(),
          &ValueEditor::TabsModel::closeDbKeys);

  m_connections = QSharedPointer<ConnectionsManager>(
      new ConnectionsManager(config, m_events));

  connect(m_events.data(), &Events::appRendered, this, [this]() {
    if (m_connections) m_connections->loadConnections();
  });

  m_bulkOperations = QSharedPointer<BulkOperations::Manager>(
      new BulkOperations::Manager(m_connections, m_python));

  connect(m_events.data(), &Events::requestBulkOperation,
          m_bulkOperations.data(),
          &BulkOperations::Manager::requestBulkOperation);

  m_consoleModel = QSharedPointer<TabViewModel>(
      new TabViewModel(getTabModelFactory<Console::Model>()));

  connect(m_events.data(), &Events::openConsole, m_consoleModel.data(),
          &TabViewModel::openTab);

  auto srvStatsFactory = [this](QSharedPointer<RedisClient::Connection> c,
                                int dbIndex, QList<QByteArray> initCmd) {
    auto rawModelPtr = new ServerStats::Model(c, dbIndex, initCmd);
    auto model = QSharedPointer<TabModel>(rawModelPtr, &QObject::deleteLater);

    QObject::connect(rawModelPtr, &ServerStats::Model::openConsoleTerminal,
                     m_events.data(), &Events::openConsole);

    return model;
  };

  m_serverStatsModel = QSharedPointer<TabViewModel>(
      new TabViewModel(srvStatsFactory));

  connect(m_events.data(), &Events::openServerStats, this,
          [this](QSharedPointer<RedisClient::Connection> c) {
            m_serverStatsModel->openTab(c);
          });

#ifdef ENABLE_EXTERNAL_FORMATTERS
  m_formattersManager = QSharedPointer<ValueEditor::ExternalFormattersManager>(
      new ValueEditor::ExternalFormattersManager());

  connect(m_formattersManager.data(),
          &ValueEditor::ExternalFormattersManager::error, this,
          [this](const QString& msg) {
            qDebug() << "External formatters:" << msg;
            m_events->log(QString("External: %1").arg(msg));
          });

  if (!m_formattersDir.isEmpty()) {
    m_formattersManager->setPath(m_formattersDir);
  }

  connect(m_events.data(), &Events::appRendered, this, [this]() {
    QtConcurrent::run([this]() {
      if (m_formattersManager) m_formattersManager->loadFormatters();
      if (m_events) emit m_events->externalFormattersLoaded();
    });
  });
#endif

  m_embeddedFormatters = QSharedPointer<ValueEditor::EmbeddedFormattersManager>(
      new ValueEditor::EmbeddedFormattersManager(m_python));

  connect(m_embeddedFormatters.data(),
          &ValueEditor::EmbeddedFormattersManager::error, this,
          [this](const QString& msg) {
            m_events->log(QString("Formatters: %1").arg(msg));
          });

  m_consoleAutocompleteModel = QSharedPointer<Console::AutocompleteModel>(
      new Console::AutocompleteModel());
}

void Application::initAppInfo() {
  setApplicationName("RedisDesktopManager");
  setApplicationVersion(QString(RDM_VERSION));
  setOrganizationDomain("redisdesktop.com");
  setOrganizationName("redisdesktop");
  setWindowIcon(QIcon(":/images/logo.png"));

  qDebug() << "TLS support:" << QSslSocket::sslLibraryVersionString();
}

void Application::initAppFonts() {
  QSettings settings;
#ifdef Q_OS_MAC
  QString defaultFontName("Helvetica Neue");
  QString defaultMonospacedFont("Monaco");
  int defaultFontSize = 12;
#elif defined(Q_OS_WINDOWS)
  QString defaultFontName("Segoe UI");
  QString defaultMonospacedFont("Consolas");
  int defaultFontSize = 11;
#else
  QString defaultFontName("Open Sans");
  QString defaultMonospacedFont("Ubuntu Mono");
  int defaultFontSize = 11;  
#endif

  int defaultValueSizeLimit = 150000;

  QString appFont = settings.value("app/appFont", defaultFontName).toString();

  if (appFont.isEmpty())
    appFont = defaultFontName;

  int appFontSize = settings.value("app/appFontSize", defaultFontSize).toInt();

  if (appFontSize < 5)
    appFontSize = defaultFontSize;

  if (appFont == "Open Sans") {
#if defined(Q_OS_LINUX)
    int result = QFontDatabase::addApplicationFont("://fonts/OpenSans.ttc");

    if (result == -1) {
      appFont = "Ubuntu";
    }
#elif defined (Q_OS_WINDOWS)
    appFont = defaultFontName;
#endif
  }

  QString valuesFont = settings.value("app/valueEditorFont", defaultMonospacedFont).toString();

  if (valuesFont.isEmpty())
    valuesFont = defaultMonospacedFont;

  int valuesFontSize = settings.value("app/valueEditorFontSize", defaultFontSize).toInt();

  if (valuesFontSize < 5)
    valuesFontSize = defaultFontSize;

  int valueSizeLimit = settings.value("app/valueSizeLimit", defaultValueSizeLimit).toInt();

  if (valueSizeLimit < 1000)
    valueSizeLimit = defaultValueSizeLimit;

  settings.setValue("app/appFont", appFont);
  settings.setValue("app/appFontSize", appFontSize);
  settings.setValue("app/valueEditorFont", valuesFont);
  settings.setValue("app/valueEditorFontSize", valuesFontSize);
  settings.setValue("app/valueSizeLimit", valueSizeLimit);

  qDebug() << "App font:" << appFont << appFontSize;
  qDebug() << "Values font:" << valuesFont;
  QFont defaultFont(appFont, appFontSize);
  QApplication::setFont(defaultFont);
}

void Application::initProxySettings() {
  QSettings settings;
  QNetworkProxyFactory::setUseSystemConfiguration(
      settings.value("app/useSystemProxy", false).toBool());
}

void Application::registerQmlTypes() {
  qmlRegisterType<SortFilterProxyModel>("rdm.models", 1, 0,
                                        "SortFilterProxyModel");
  qmlRegisterType<SyntaxHighlighter>("rdm.models", 1, 0, "SyntaxHighlighter");
  qmlRegisterType<TextCharFormat>("rdm.models", 1, 0, "TextCharFormat");
  qRegisterMetaType<ServerConfig>();
}

void Application::registerQmlRootObjects() {
  m_engine.rootContext()->setContextProperty("appEvents", m_events.data());
  m_engine.rootContext()->setContextProperty("qmlUtils", m_qmlUtils.data());
  m_engine.rootContext()->setContextProperty("connectionsManager",
                                             m_connections.data());
  m_engine.rootContext()->setContextProperty("keyFactory", m_keyFactory.data());
  m_engine.rootContext()->setContextProperty("valuesModel", m_keyValues.data());
#ifdef ENABLE_EXTERNAL_FORMATTERS
  m_engine.rootContext()->setContextProperty("formattersManager",
                                             m_formattersManager.data());
#endif
  m_engine.rootContext()->setContextProperty("embeddedFormattersManager",
                                             m_embeddedFormatters.data());
  m_engine.rootContext()->setContextProperty("consoleModel",
                                             m_consoleModel.data());
  m_engine.rootContext()->setContextProperty("serverStatsModel",
                                             m_serverStatsModel.data());
  m_engine.rootContext()->setContextProperty("bulkOperations",
                                             m_bulkOperations.data());
  m_engine.rootContext()->setContextProperty("consoleAutocompleteModel",
                                             m_consoleAutocompleteModel.data());
}

void Application::initQml() {
  if (m_renderingBackend == "auto") {
    QQuickWindow::setSceneGraphBackend(QSGRendererInterface::Software);
  } else {
    QQuickWindow::setSceneGraphBackend(m_renderingBackend);
  }

  registerQmlTypes();
  registerQmlRootObjects();

  try {
    m_engine.load(QUrl(QStringLiteral("qrc:///app.qml")));
  } catch (...) {
    qDebug() << "Failed to load app window. Retrying with software renderer...";
    QQuickWindow::setSceneGraphBackend(QSGRendererInterface::Software);
    m_engine.load(QUrl(QStringLiteral("qrc:///app.qml")));
  }

  updatePalette();
  connect(this, &QGuiApplication::paletteChanged, this, &Application::updatePalette);

  qDebug() << "Rendering backend:" << QQuickWindow::sceneGraphBackend();

  emit m_events->appRendered();
}

void Application::initPython() {
  m_python = QSharedPointer<QPython>(new QPython(this, 1, 5));
  m_python->addImportPath("qrc:/python/");

#ifdef Q_OS_MACOS
  m_python->addImportPath(applicationDirPath() + "/../Resources/py");
#else
  m_python->addImportPath(applicationDirPath());
#endif
}

void Application::installTranslator() {
  QSettings settings;
  QString preferredLocale = settings.value("app/locale", "system").toString();

  QString locale;

  if (preferredLocale == "system") {
    settings.setValue("app/locale", "system");
    locale = QLocale::system().uiLanguages().first().replace("-", "_");

    qDebug() << QLocale::system().uiLanguages();

    if (locale.isEmpty() || locale == "C") locale = "en_US";

    qDebug() << "Detected locale:" << locale;
  } else {
    locale = preferredLocale;
  }

  QTranslator* translator = new QTranslator((QObject*)this);
  if (translator->load(QString(":/translations/rdm_") + locale)) {
    qDebug() << "Load translations file for locale:" << locale;
    QCoreApplication::installTranslator(translator);
  } else {
    delete translator;
  }
}

void Application::processCmdArgs() {
  QCommandLineParser parser;
  QCommandLineOption settingsDir("settings-dir",
                                 "(Optional) Directory where RDM looks/saves "
                                 ".rdm directory with connections.json file",
                                 "settingsDir", QDir::homePath());
  QCommandLineOption formattersDir(
      "formatters-dir",
      "(Optional) Directory where RDM looks for native value formatters",
      "formattersDir",
#ifdef Q_OS_WIN32
      QString("%1/formatters").arg(QCoreApplication::applicationDirPath()));
#elif defined Q_OS_MACOS
      QString("%1/.rdm/formatters").arg(QDir::homePath()));
#else
      QString());
#endif

  QCommandLineOption renderingBackend(
      "rendering-backend",
      "(Optional) QML rendering backend [software|opengl|d3d12|'']",
      "renderingBackend", "auto");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addOption(settingsDir);
  parser.addOption(formattersDir);
  parser.addOption(renderingBackend);
  parser.process(*this);

  m_settingsDir = parser.value(settingsDir);
  m_formattersDir = parser.value(formattersDir);
  m_renderingBackend = parser.value(renderingBackend);
}

void Application::updatePalette()
{
    if (m_engine.rootObjects().size() == 0) {
        qWarning() << "Cannot update palette. Root object is not loaded.";
        return;
    }

    m_engine.rootObjects().at(0)->setProperty(
                "palette", QGuiApplication::palette());
}
