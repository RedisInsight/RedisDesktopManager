#include "app.h"

#include <QtQml>
#include <QUrl>
#include <QSysInfo>
#include <QQmlContext>
#include <QSettings>
#include <QMessageBox>
#include <QQuickWindow>
#include <QNetworkProxyFactory>
#include <easylogging++.h>
#include <qredisclient/redisclient.h>

#include "logger.h"
#include "qmlutils.h"
#include "common/tabviewmodel.h"
#include "models/connectionconf.h"
#include "models/configmanager.h"
#include "models/connectionsmanager.h"
#include "models/key-models/keyfactory.h"
#include "modules/updater/updater.h"
#include "modules/value-editor/valueviewmodel.h"
#include "modules/value-editor/tabsmodel.h"
#include "modules/value-editor/sortfilterproxymodel.h"
#include "modules/value-editor/formattersmanager.h"
#include "modules/console/consolemodel.h"
#include "modules/server-stats/serverstatsmodel.h"
#include "modules/bulk-operations/bulkoperationsmanager.h"


INITIALIZE_EASYLOGGINGPP


Application::Application(int &argc, char **argv)
    : QApplication(argc, argv),
      m_engine(this),
      m_qmlUtils(QSharedPointer<QmlUtils>(new QmlUtils())),      
      m_logger(nullptr)
{
    // Init components required for models and qml
    initLog();
    initAppInfo();
    processCmdArgs();
    initAppFonts();    
    initRedisClient();
    initUpdater();    
    installTranslator();
}

void Application::initModels()
{
    initConnectionsManager();

    m_consoleModel = QSharedPointer<TabViewModel>(new TabViewModel(getTabModelFactory<Console::Model>()));

    connect(m_connections.data(), &ConnectionsManager::openConsole,
            m_consoleModel.data(), &TabViewModel::openTab);

    m_serverStatsModel = QSharedPointer<TabViewModel>(new TabViewModel(getTabModelFactory<ServerStats::Model>()));

    connect(m_connections.data(), &ConnectionsManager::openServerStats,
            m_serverStatsModel.data(), &TabViewModel::openTab);

    m_formattersManager = QSharedPointer<ValueEditor::FormattersManager>(new ValueEditor::FormattersManager());
    m_formattersManager->loadFormatters();
}

void Application::initAppInfo()
{
    setApplicationName("RedisDesktopManager");
    setApplicationVersion(QString(RDM_VERSION));
    setOrganizationDomain("redisdesktop.com");
    setOrganizationName("redisdesktop");
    setWindowIcon(QIcon(":/images/logo.png"));
}

void Application::initAppFonts()
{
    QSettings settings;
#ifdef Q_OS_MAC    
    QString defaultFontName("Helvetica Neue");
    int defaultFontSize = 12;
#else 
    QString defaultFontName("Open Sans");
    int defaultFontSize = 11;
#endif    
    
    QString appFont = settings.value("app/appFont", defaultFontName).toString();
    int appFontSize = settings.value("app/appFontSize", defaultFontSize).toInt();

#ifdef Q_OS_LINUX
    if (appFont == "Open Sans") {
        int result = QFontDatabase::addApplicationFont("://fonts/OpenSans.ttc");

        if (result == -1) {
            appFont = "Ubuntu";
        }
    }
#endif

    qDebug() << "App font:" << appFont << appFontSize;
    QFont defaultFont(appFont, appFontSize);
    QApplication::setFont(defaultFont);
}

void Application::initProxySettings()
{
    QSettings settings;
    QNetworkProxyFactory::setUseSystemConfiguration(settings.value("app/useSystemProxy", false).toBool());
}

void Application::registerQmlTypes()
{    
    qmlRegisterType<SortFilterProxyModel>("rdm.models", 1, 0, "SortFilterProxyModel");        
    qRegisterMetaType<ServerConfig>();    
}

void Application::registerQmlRootObjects()
{        
    m_engine.rootContext()->setContextProperty("binaryUtils", m_qmlUtils.data()); // TODO: Remove legacy name usage in qml
    m_engine.rootContext()->setContextProperty("qmlUtils", m_qmlUtils.data());
    m_engine.rootContext()->setContextProperty("connectionsManager", m_connections.data());
    m_engine.rootContext()->setContextProperty("viewModel", m_keyValues.data()); // TODO: Remove legacy name usage in qml    
    m_engine.rootContext()->setContextProperty("valuesModel", m_keyValues.data());
    m_engine.rootContext()->setContextProperty("formattersManager", m_formattersManager.data());
    m_engine.rootContext()->setContextProperty("consoleModel", m_consoleModel.data());
    m_engine.rootContext()->setContextProperty("serverStatsModel", m_serverStatsModel.data());
    m_engine.rootContext()->setContextProperty("appLogger", m_logger);
    m_engine.rootContext()->setContextProperty("bulkOperations", m_bulkOperations.data());
}

void Application::initQml()
{             
    if (m_renderingBackend == "auto") {
        #if defined(Q_OS_WIN) || defined(Q_OS_LINUX)        
        // Use software renderer on Windows & Linux by default
        QQuickWindow::setSceneGraphBackend(QSGRendererInterface::Software);
        #endif
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

    qDebug() << "Rendering backend:" << QQuickWindow::sceneGraphBackend();
}

void Application::initLog()
{
    el::Configurations defaultConf;
    defaultConf.setToDefault();
    defaultConf.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
    defaultConf.setGlobally(el::ConfigurationType::ToFile, "false");
    el::Loggers::reconfigureLogger("default", defaultConf);

    el::Loggers::removeFlag(el::LoggingFlag::NewLineForContainer);
    el::Helpers::installLogDispatchCallback<LogHandler>("LogHandler");
    m_logger = el::Helpers::logDispatchCallback<LogHandler>("LogHandler");

    if (!m_logger) {
        LOG(ERROR) << "App log: ERROR";
    } else {
        LOG(INFO) << "App log init: OK";
    }
}

void Application::initConnectionsManager()
{
    //connection manager
    ConfigManager confManager(m_settingsDir);
    if (confManager.migrateOldConfig("connections.xml", "connections.json")) {
        LOG(INFO) << "Migrate connections.xml to connections.json";
    }

    QString config = confManager.getApplicationConfigPath("connections.json");

    if (config.isNull()) {
        QMessageBox::critical(nullptr,
            QObject::tr("Settings directory is not writable"),
            QString(QObject::tr("RDM can't save connections file to settings directory. "
                    "Please change file permissions or restart RDM as administrator."))
            );

        throw std::runtime_error("invalid connections config");
    }

    QSharedPointer<KeyFactory> keyFactory(new KeyFactory());

    m_keyValues = QSharedPointer<ValueEditor::TabsModel>(
                    new ValueEditor::TabsModel(
                        keyFactory.staticCast<ValueEditor::AbstractKeyFactory>()
                    )
                );

    m_connections = QSharedPointer<ConnectionsManager>(new ConnectionsManager(config));

    m_bulkOperations = QSharedPointer<BulkOperations::Manager>(new BulkOperations::Manager(m_connections));

    QObject::connect(m_connections.data(), &ConnectionsManager::openValueTab,
                     m_keyValues.data(), &ValueEditor::TabsModel::openTab);
    QObject::connect(m_connections.data(), &ConnectionsManager::newKeyDialog,
                     m_keyValues.data(), &ValueEditor::TabsModel::openNewKeyDialog);
    QObject::connect(m_connections.data(), &ConnectionsManager::closeDbKeys,
                     m_keyValues.data(), &ValueEditor::TabsModel::closeDbKeys);
    QObject::connect(m_connections.data(), &ConnectionsManager::requestBulkOperation,
                     m_bulkOperations.data(), &BulkOperations::Manager::requestBulkOperation);
}

void Application::initUpdater()
{
    m_updater = QSharedPointer<Updater>(new Updater());
    connect(m_updater.data(), SIGNAL(updateUrlRetrived(QString &)), this, SLOT(OnNewUpdateAvailable(QString &)));
}

void Application::installTranslator()
{
    QSettings settings;
    QString preferredLocale = settings.value("app/locale", "system").toString();

    QString locale;

    if (preferredLocale == "system") {
        settings.setValue("app/locale", "system");
        locale = QLocale::system().uiLanguages().first().replace( "-", "_" );

        qDebug() << QLocale::system().uiLanguages();

        if (locale.isEmpty() || locale == "C")
            locale = "en_US";

        qDebug() << "Detected locale:" << locale;
    } else {
        locale = preferredLocale;
    }

    QTranslator* translator = new QTranslator((QObject *)this);
    if (translator->load( QString( ":/translations/rdm_" ) + locale ))
    {
        qDebug() << "Load translations file for locale:" << locale;
        QCoreApplication::installTranslator( translator );
    } else {
        delete translator;
    }
}

void Application::processCmdArgs()
{
    QCommandLineParser parser;    
    QCommandLineOption settingsDir(
            "settings-dir",
             "(Optional) Directory where RDM looks/saves .rdm directory with connections.json file",
             "settingsDir",
             QDir::homePath()
    );
    QCommandLineOption renderingBackend(
            "rendering-backend",
             "(Optional) QML rendering backend [software|opengl|d3d12|'']",
             "renderingBackend",
             "auto"
    );
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(settingsDir);
    parser.addOption(renderingBackend);
    parser.process(*this);

    m_settingsDir = parser.value(settingsDir);
    m_renderingBackend = parser.value(renderingBackend);
}

void Application::OnNewUpdateAvailable(QString &url)
{
    QMessageBox::information(nullptr, "New update available",
        QString(QObject::tr("Please download new version of Redis Desktop Manager: %1")).arg(url));
}
