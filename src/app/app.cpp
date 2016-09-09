#include "app.h"

#include <QtQml>
#include <QUrl>
#include <QSysInfo>
#include <QQmlContext>
#include <QMessageBox>
#include <easylogging++.h>
#include <googlemp.h>
#include <qredisclient/redisclient.h>

#include "logger.h"
#include "qmlutils.h"
#include "models/connectionconf.h"
#include "models/configmanager.h"
#include "models/connectionsmanager.h"
#include "models/key-models/keyfactory.h"
#include "modules/updater/updater.h"
#include "modules/value-editor/valueviewmodel.h"
#include "modules/value-editor/viewmodel.h"
#include "modules/value-editor/sortfilterproxymodel.h"
#include "modules/console/consoleviewmodel.h"
#include "modules/bulk-operations/bulkoperationsmanager.h"


INITIALIZE_EASYLOGGINGPP

static QObject *analytics_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    GoogleMP *gmp = GoogleMP::instance();
    return gmp;
}

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv),
      m_qmlUtils(QSharedPointer<QmlUtils>(new QmlUtils())),      
      m_logger(nullptr)
{
    // Init components required for models and qml
    initLog();
    initAppInfo();
    initAppFonts();
    initAppAnalytics();
    initRedisClient();
    initUpdater();    
}

void Application::initModels()
{
    initConnectionsManager();

    m_consoleModel = QSharedPointer<Console::ViewModel>(new Console::ViewModel());

    connect(m_connections.data(), &ConnectionsManager::openConsole,
            m_consoleModel.data(), &Console::ViewModel::openConsole);
}

void Application::initAppInfo()
{
    setApplicationName("Redis Desktop Manager");
    setApplicationVersion(QString(RDM_VERSION));
    setOrganizationDomain("redisdesktop.com");
    setOrganizationName("redisdesktop");
}

void Application::initAppFonts()
{
    QFontDatabase::addApplicationFont("://fonts/OpenSans-Regular.ttc");
    QFont defaultFont("OpenSans", 11);
    QApplication::setFont(defaultFont);
}

void Application::initAppAnalytics()
{
    GoogleMP::startSession(QDateTime::currentMSecsSinceEpoch());
    GoogleMP::instance()->reportEvent("rdm:cpp", "app start", "");
}

void Application::registerQmlTypes()
{
    qmlRegisterType<ValueEditor::ValueViewModel>("rdm.models", 1, 0, "ValueViewModel");   
    qmlRegisterType<SortFilterProxyModel>("rdm.models", 1, 0, "SortFilterProxyModel");
    qmlRegisterSingletonType<GoogleMP>("MeasurementProtocol", 1, 0, "Analytics", analytics_singletontype_provider);
    qRegisterMetaType<ServerConfig>();
}

void Application::registerQmlRootObjects()
{        
    m_engine.rootContext()->setContextProperty("binaryUtils", m_qmlUtils.data()); // TODO: Remove legacy name usage in qml
    m_engine.rootContext()->setContextProperty("qmlUtils", m_qmlUtils.data());
    m_engine.rootContext()->setContextProperty("connectionsManager", m_connections.data());
    m_engine.rootContext()->setContextProperty("viewModel", m_keyValues.data()); // TODO: Remove legacy name usage in qml    
    m_engine.rootContext()->setContextProperty("valuesModel", m_keyValues.data());
    m_engine.rootContext()->setContextProperty("consoleModel", m_consoleModel.data());
    m_engine.rootContext()->setContextProperty("appLogger", m_logger);
    m_engine.rootContext()->setContextProperty("bulkOperations", m_bulkOperations.data());
}

void Application::initQml()
{
    registerQmlTypes();
    registerQmlRootObjects();
    m_engine.load(QUrl(QStringLiteral("qrc:///app.qml")));
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
    ConfigManager confManager;
    if (confManager.migrateOldConfig("connections.xml", "connections.json")) {
        LOG(INFO) << "Migrate connections.xml to connections.json";
    }

    QString config = confManager.getApplicationConfigPath("connections.json");

    if (config.isNull()) {
        QMessageBox::critical(nullptr,
            "Settings directory is not writable",
            QString("Program can't save connections file to settings dir."
                    "Please change permissions or restart this program "
                    " with administrative privileges")
            );

        throw std::runtime_error("invalid connections config");
    }

    QSharedPointer<KeyFactory> keyFactory(new KeyFactory());

    m_keyValues = QSharedPointer<ValueEditor::ViewModel>(
                    new ValueEditor::ViewModel(
                        keyFactory.staticCast<ValueEditor::AbstractKeyFactory>()
                    )
                );

    m_connections = QSharedPointer<ConnectionsManager>(new ConnectionsManager(config));

    m_bulkOperations = QSharedPointer<BulkOperations::Manager>(new BulkOperations::Manager(m_connections));

    QObject::connect(m_connections.data(), &ConnectionsManager::openValueTab,
                     m_keyValues.data(), &ValueEditor::ViewModel::openTab);
    QObject::connect(m_connections.data(), &ConnectionsManager::newKeyDialog,
                     m_keyValues.data(), &ValueEditor::ViewModel::openNewKeyDialog);
    QObject::connect(m_connections.data(), &ConnectionsManager::closeDbKeys,
                     m_keyValues.data(), &ValueEditor::ViewModel::closeDbKeys);
    QObject::connect(m_connections.data(), &ConnectionsManager::requestBulkOperation,
                     m_bulkOperations.data(), &BulkOperations::Manager::requestBulkOperation);
}

void Application::initUpdater()
{
    m_updater = QSharedPointer<Updater>(new Updater());
    connect(m_updater.data(), SIGNAL(updateUrlRetrived(QString &)), this, SLOT(OnNewUpdateAvailable(QString &)));
}

void Application::OnNewUpdateAvailable(QString &url)
{
    QMessageBox::information(nullptr, "New update available",
        QString("Please download new version of Redis Desktop Manager: %1").arg(url));
}
