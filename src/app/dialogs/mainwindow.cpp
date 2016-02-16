#include "mainwindow.h"

#include <QMenu>
#include <QFileDialog>
#include <QStatusBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QDialog>
#include <QMovie>
#include <QDesktopWidget>
#include <easylogging++.h>
#include <qredisclient/redisclient.h>
#include <googlemp.h>

#include "app/models/configmanager.h"
#include "app/models/connectionsmanager.h"
#include "app/models/key-models/keyfactory.h"
#include "app/dialogs/connect.h"
#include "app/dialogs/quickstartdialog.h"
#include "modules/updater/updater.h"
#include "modules/value-editor/view.h"
#include "modules/value-editor/viewmodel.h"
#include "modules/console/logtab.h"

INITIALIZE_EASYLOGGINGPP

MainWin::MainWin(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    initRedisClient();
    initSystemConsole();

    QDesktopWidget* desktop = QApplication::desktop();
    QRect scr = desktop->screenGeometry();

    /* Smart resize on big screens */
    float wRatio = geometry().width()/(float)scr.width();
    float hRatio = geometry().height()/(float)scr.height();

    LOG(DEBUG) << "Current ratio width:" << wRatio;
    LOG(DEBUG) << "Current ratio height:" << hRatio;

    float minimumRatioW = 0.6;
    float minimumRatioH = 0.7;

    if (wRatio < minimumRatioW
            || hRatio < minimumRatioH) {
        LOG(DEBUG) << "Resize main window";
        setGeometry(geometry().x(), geometry().y(),
                    (int)(scr.width() * minimumRatioW),
                    (int)(scr.height() * minimumRatioH));
    } else if (hRatio > 1 || wRatio > 1) {
        LOG(DEBUG) << "Ratio > 1.0. Resize main window.";
        setMinimumSize((int)(scr.width() * 0.6),
                       (int)(scr.height() * 0.6));
        setGeometry(geometry().x(), geometry().y(),
                    (int)(scr.width() * 0.8),
                    (int)(scr.height() * 0.8));
    }
    move(scr.center() - rect().center());

    initConnectionsTreeView();
    initContextMenus();
    initFormButtons();    
    initUpdater();
}

void MainWin::initConnectionsTreeView()
{
    //connection manager
    ConfigManager confManager;
    if (confManager.migrateOldConfig("connections.xml", "connections.json")) {
        LOG(INFO) << "Migrate connections.xml to connections.json";
    }

    QString config = confManager.getApplicationConfigPath("connections.json");

    if (config.isNull()) {
        QMessageBox::warning(this,
            "Settings directory is not writable",
            QString("Program can't save connections file to settings dir."
                    "Please change permissions or restart this program "
                    " with administrative privileges")
            );

        exit(1);
    }

    initValuesView();

    connections = QSharedPointer<ConnectionsManager>(
                    new ConnectionsManager(config, *ui.tabWidget, m_keyValues)
                );

    QObject::connect(connections.data(), &ConnectionsManager::editConnection,
                     this, &MainWin::OnEditConnectionClick);

    if (connections->size() == 0) {
        QTimer::singleShot(1000, this, SLOT(showQuickStartDialog()));
    }

    ui.serversTreeView->setModel(connections.data());

    // fix initial widgets sizes
    ui.mainSplitter->setSizes(QList<int>() << 300 << 750);    
}

void MainWin::initContextMenus()
{
    // TODO: move to custom QMenu class
    connectionsMenu = QSharedPointer<QMenu>(new QMenu());
    connectionsMenu->addAction(QIcon(":/images/import.png"), "Import Connections", this, SLOT(OnImportConnectionsClick()));
    connectionsMenu->addAction(QIcon(":/images/export.png"), "Export Connections", this, SLOT(OnExportConnectionsClick()));
    connectionsMenu->addSeparator();    

    ui.pbImportConnections->setMenu(connectionsMenu.data());
}

void MainWin::initFormButtons()
{
    connect(ui.pbAddServer, SIGNAL(clicked()), SLOT(OnAddConnectionClick()));    
    connect(ui.pbImportConnections, SIGNAL(clicked()), SLOT(OnImportConnectionsClick()));
}

void MainWin::initUpdater()
{
    updater = QSharedPointer<Updater>(new Updater());
    connect(updater.data(), SIGNAL(updateUrlRetrived(QString &)), this, SLOT(OnNewUpdateAvailable(QString &)));
}

void MainWin::initSystemConsole()
{
    QSharedPointer<Console::LogTab> tab(new Console::LogTab());
    ui.tabWidget->addTab(tab);

    el::Configurations defaultConf;
    defaultConf.setToDefault();
    defaultConf.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
    defaultConf.setGlobally(el::ConfigurationType::ToFile, "false");
    el::Loggers::reconfigureLogger("default", defaultConf);

    el::Loggers::removeFlag(el::LoggingFlag::NewLineForContainer);
    el::Helpers::installLogDispatchCallback<Console::LogHandler>("LogHandler");
    Console::LogHandler* logHandler = el::Helpers::logDispatchCallback<Console::LogHandler>("LogHandler");
    logHandler->setOutputTab(tab.toWeakRef());

    LOG(INFO) << "Init app log";
}

void MainWin::initValuesView()
{
    QSharedPointer<KeyFactory> keyFactory(new KeyFactory());

    m_keyValues = QSharedPointer<ValueEditor::ViewModel>(
                    new ValueEditor::ViewModel(
                        keyFactory.staticCast<ValueEditor::AbstractKeyFactory>()
                    )
                );

    ValueEditor::View* valueView = new ValueEditor::View(m_keyValues);
    valueView->setParent(ui.qmlParent);

    QBoxLayout * layout = new QBoxLayout(QBoxLayout::LeftToRight, ui.qmlParent);
    layout->setMargin(0);
    layout->addWidget(valueView);
    ui.qmlParent->setLayout(layout);
    ui.qmlParent->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    ui.splitter->setSizes(QList<int>() << 0 << 100);
}

void MainWin::openConnectionDialog(ConnectionConfig config)
{
    QScopedPointer<ConnectionWindow> connectionDialog(new ConnectionWindow(connections.toWeakRef(), this));
    connectionDialog->setModal(true);
    connectionDialog->setWindowState(Qt::WindowActive);
#if defined(Q_OS_LINUX) || defined(Q_OS_WINDOWS)
    connectionDialog->move(this->pos().x() + this->width()/2 - connectionDialog->width() / 2,
                           this->pos().y() + this->height()/2 - connectionDialog->height() / 2);
#endif

    if (!config.isNull())
        connectionDialog->setConnectionConfig(config);

    connectionDialog->exec();
}

void MainWin::showQuickStartDialog()
{
    QScopedPointer<QuickStartDialog> dialog(new QuickStartDialog(this));
    dialog->setWindowState(Qt::WindowActive);
    dialog->exec();
}

void MainWin::OnConsoleStateChanged()
{
//    ui.systemConsole->setVisible(!ui.systemConsole->isVisible());
}

void MainWin::OnAddConnectionClick()
{
    openConnectionDialog();
}

void MainWin::OnEditConnectionClick(ConnectionConfig config)
{
    openConnectionDialog(config);
}

void MainWin::OnNewUpdateAvailable(QString &url)
{
    QMessageBox::information(this, "New update available", 
        QString("Please download new version of Redis Desktop Manager: %1").arg(url));
}

void MainWin::OnImportConnectionsClick()
{
    GoogleMP::instance()->showScreen("import-connections");

    QString fileName = QFileDialog::getOpenFileName(this, "Import Connections", "", tr("Xml Files and JSON files (*.xml *.json)"));

    if (fileName.isEmpty()) {
        return;
    }

    if (connections->importConnections(fileName)) {
        QMessageBox::information(this, "Connections imported", "Connections imported from connections file");
    } else {
        QMessageBox::warning(this, "Can't import connections", "Select valid file for import");
    }       
}

void MainWin::OnExportConnectionsClick()
{
    GoogleMP::instance()->showScreen("export-connections");

    QString fileName = QFileDialog::getSaveFileName(this, "Export Connections to JSON", "", tr("Json Files (*.json)"));

    if (fileName.isEmpty()) {
        return;
    }

    if (connections->saveConnectionsConfigToFile(fileName)) {
        QMessageBox::information(this, "Connections exported", "Connections exported in selected file");
    } else {
        QMessageBox::warning(this, "Can't export connections", "Select valid file name for export");
    }
}
