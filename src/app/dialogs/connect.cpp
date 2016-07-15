#include "connect.h"
#include <QtWidgets/QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <qredisclient/connection.h>
#include <googlemp.h>
#include "app/models/connectionsmanager.h"

ConnectionWindow::ConnectionWindow(QWeakPointer<ConnectionsManager> manager, QWidget *parent)
    : QDialog(parent), m_inEditMode(false), m_manager(manager)
{
    ui.setupUi(this);

    this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);       
    this->setModal(true);

    ui.validationWarning->hide();

    // connect slots to signals
    connect(ui.okButton, SIGNAL(clicked()), this, SLOT(OnOkButtonClick()));        
    connect(ui.selectPrivateKeyPath, &QPushButton::clicked, this, [this]() {
        OnBrowseFileClick(ui.privateKeyPath, "Select private key file", tr("All Files (*.*)"));
    });
    connect(ui.sslCACertButton, &QPushButton::clicked, this, [this]() {
        OnBrowseFileClick(ui.sslCACertEdit, "Select CACert file", tr("All Files (*.pem *.crt)"));
    });
    connect(ui.sslLocalCertButton, &QPushButton::clicked, this, [this]() {
        OnBrowseFileClick(ui.sslLocalCertEdit, "Select Local Cert file", tr("All Files (*.pem *.crt)"));
    });
    connect(ui.sslPrivateKeyButton, &QPushButton::clicked, this, [this]() {
        OnBrowseFileClick(ui.sslPrivateKeyEdit, "Select Private key file", tr("All Files (*.pem *.key)"));
    });
    connect(ui.testConnectionButton, SIGNAL(clicked()), this, SLOT(OnTestConnectionButtonClick()));
    connect(ui.showPasswordCheckbox, SIGNAL(stateChanged(int)), this, SLOT(OnShowPasswordCheckboxChanged(int)));

    ui.namespaceSeparator->setText(QString(ConnectionConfig::DEFAULT_NAMESPACE_SEPARATOR));
    ui.keysPattern->setText(QString(ConnectionConfig::DEFAULT_KEYS_GLOB_PATTERN));
    ui.connectionTimeout->setValue(ConnectionConfig::DEFAULT_TIMEOUT_IN_MS / 1000);
    ui.executionTimeout->setValue(ConnectionConfig::DEFAULT_TIMEOUT_IN_MS / 1000);

    GoogleMP::instance()->showScreen("connection-dialog");
}

void ConnectionWindow::setConnectionConfig(const ConnectionConfig& config)
{
    m_config = config;
    m_inEditMode = true;
    loadValuesFromConfig(m_config);
}

void ConnectionWindow::loadValuesFromConfig(const ConnectionConfig& config)
{
    m_inEditMode = true;

    ui.nameEdit->setText(config.name());
    ui.hostEdit->setText(config.host());
    ui.portSpinBox->setValue(config.port());
    ui.authEdit->setText(config.auth());
    ui.namespaceSeparator->setText(config.namespaceSeparator());
    ui.connectionTimeout->setValue(config.connectionTimeout()/1000);
    ui.executionTimeout->setValue(config.executeTimeout()/1000);

    if (!config.keysPattern().isEmpty()) {
        ui.keysPattern->setText(config.keysPattern());
    }

    if (config.useSshTunnel()) {
        ui.useSshTunnel->setCheckState(Qt::Checked);
        ui.sshHost->setText(config.sshHost());
        ui.sshUser->setText(config.sshUser());
        ui.sshPass->setText(config.sshPassword());
        ui.sshPort->setValue(config.sshPort());
        ui.privateKeyPath->setText(config.getSshPrivateKeyPath());

        ui.sshKeysGroup->setChecked(false);
        ui.sshPasswordGroup->setChecked(false);

        if (!ui.sshPass->text().isEmpty()) {
            ui.sshPasswordGroup->setChecked(true);
        }

        if (!ui.privateKeyPath->text().isEmpty()) {
            ui.sshKeysGroup->setChecked(true);
        }
    }

    if (config.useSsl()) {
        ui.useSsl->setCheckState(Qt::Checked);
        ui.sslCACertEdit->setText(config.sslCaCertPath());
        ui.sslPrivateKeyEdit->setText(config.sslPrivateKeyPath());
        ui.sslLocalCertEdit->setText(config.sslLocalCertPath());
    }
}

void ConnectionWindow::markFieldInvalid(QWidget* w)
{
    w->setStyleSheet("border: 1px solid red;");
}

void ConnectionWindow::markFieldValid(QWidget* w)
{
    w->setStyleSheet("");
}

void ConnectionWindow::OnOkButtonClick()
{
    ui.validationWarning->hide();

    if (!isFormDataValid()) {
        ui.validationWarning->show();
        return;    
    }

    ConnectionConfig conf = getConectionConfigFromFormData();

    if (!m_manager) // some error occurred
        close();

    auto manager = m_manager.toStrongRef();

    if (m_inEditMode) {
        conf.setOwner(m_config.getOwner());
        manager->updateConnection(conf);
    } else {
        manager->addNewConnection(conf);
    }
    
    close();
}

void ConnectionWindow::OnShowPasswordCheckboxChanged(int state)
{
    if (state == Qt::Unchecked) {
        ui.sshPass->setEchoMode(QLineEdit::Password);
    } else {
        ui.sshPass->setEchoMode(QLineEdit::Normal);
    }
}

void ConnectionWindow::OnBrowseFileClick(QLineEdit* target, QString msg, QString fileFilter)
{
    QString fileName = QFileDialog::getOpenFileName(this, msg, "", fileFilter);

    if (fileName.isEmpty())
        return;

    target->setText(fileName);
}

void ConnectionWindow::OnTestConnectionButtonClick()
{
    ui.validationWarning->hide();

    if (!isFormDataValid()) {
        ui.validationWarning->show();
        return;
    }

    ui.testConnectionButton->setIcon(QIcon(":/images/wait.png"));

    ConnectionConfig config = getConectionConfigFromFormData();
    config.setConnectionTimeout(8000);

    RedisClient::Connection testConnection(config);

    try {
        if (testConnection.connect()) {
            QMessageBox::information(this, "Successful connection", "Successful connection to redis-server");
        } else {
            QMessageBox::warning(this, "Can't connect to redis-server", "Can't connect to redis-server");
        }
    } catch (const RedisClient::Connection::Exception& e) {
        QMessageBox::warning(this, "Error occurred", QString("Error: %1. Please increase timeouts and try again.").arg(e.what()));
    }

    ui.testConnectionButton->setIcon(QIcon());
}

bool ConnectionWindow::isFormDataValid()
{    
    return isConnectionSettingsValid() 
        && isSslSettingsValid()
        && isSshSettingsValid()
        && isAdvancedSettingsValid();
}

bool ConnectionWindow::isConnectionSettingsValid()
{
    markFieldValid(ui.nameEdit);
    markFieldValid(ui.hostEdit);

    bool isValid = !ui.nameEdit->text().isEmpty()
        && !ui.hostEdit->text().isEmpty()
        && ui.portSpinBox->value() > 0;

    if (isValid)
        return true;    

    if (ui.nameEdit->text().isEmpty())
        markFieldInvalid(ui.nameEdit);

    if (ui.hostEdit->text().isEmpty())
        markFieldInvalid(ui.hostEdit);

    return false;
}

bool ConnectionWindow::isAdvancedSettingsValid()
{
    markFieldValid(ui.namespaceSeparator);
    markFieldValid(ui.keysPattern);

    bool isValid = !ui.keysPattern->text().isEmpty()
            && ui.connectionTimeout->value() >= 10
            && ui.executionTimeout->value() >= 10;


    if (isValid)
        return true;

    if (ui.keysPattern->text().isEmpty())
        markFieldInvalid(ui.keysPattern);

    if (ui.connectionTimeout->value() < 10)
        markFieldInvalid(ui.connectionTimeout);

    if (ui.executionTimeout->value() < 10)
        markFieldInvalid(ui.executionTimeout);

    return false;
}

bool ConnectionWindow::isSshSettingsValid()
{
    markFieldValid(ui.sshHost);
    markFieldValid(ui.sshUser);
    markFieldValid(ui.sshPass);
    markFieldValid(ui.privateKeyPath);

    if (!isSshTunnelUsed())
        return true;    

    bool isValid =  !ui.sshHost->text().isEmpty()  
            && !ui.sshUser->text().isEmpty() 
            && !(ui.sshPass->text().isEmpty() && ui.privateKeyPath->text().isEmpty())
            && (
                (ui.sshPasswordGroup->isChecked() && !ui.sshPass->text().isEmpty())
                ||
                (ui.sshKeysGroup->isChecked() && !ui.privateKeyPath->text().isEmpty()
                 && QFile::exists(ui.privateKeyPath->text()))
                )
            && ui.sshPort->value() > 0;

    if (isValid)
        return true;    

    if (ui.sshHost->text().isEmpty())
       markFieldInvalid(ui.sshHost);

    if (ui.sshUser->text().isEmpty())
        markFieldInvalid(ui.sshUser);

    if (ui.sshPasswordGroup->isChecked() && ui.sshPass->text().isEmpty())
        markFieldInvalid(ui.sshPass);

    if (ui.sshKeysGroup->isChecked() && ui.privateKeyPath->text().isEmpty())
        markFieldInvalid(ui.privateKeyPath);

    if (!ui.sshPasswordGroup->isChecked() && !ui.sshKeysGroup->isChecked()) {
        markFieldInvalid(ui.sshPass);
        markFieldInvalid(ui.privateKeyPath);
    }

    return false;
}

bool ConnectionWindow::isSshTunnelUsed()
{
    return ui.useSshTunnel->checkState() == Qt::Checked;
}

bool ConnectionWindow::isSslUsed()
{
    return ui.useSsl->checkState() == Qt::Checked;
}

bool ConnectionWindow::isSslSettingsValid()
{
    if (!isSslUsed())
        return true;

    return true;
}

ConnectionConfig ConnectionWindow::getConectionConfigFromFormData()
{    
    ConnectionConfig conf(ui.hostEdit->text().trimmed(),
                          ui.authEdit->text().isEmpty() ? "" : ui.authEdit->text(),
                          (uint)ui.portSpinBox->value(),
                          ui.nameEdit->text().trimmed());

    conf.setNamespaceSeparator(ui.namespaceSeparator->text());
    conf.setConnectionTimeout(ui.connectionTimeout->value() * 1000);
    conf.setExecutionTimeout(ui.executionTimeout->value() * 1000);

    if (!ui.keysPattern->text().isEmpty())
        conf.setKeysPattern(ui.keysPattern->text());

    if (isSshTunnelUsed()) {
        conf.setSshTunnelSettings(
            ui.sshHost->text().trimmed(), 
            ui.sshUser->text().trimmed(), 
            (ui.sshPasswordGroup->isChecked()? ui.sshPass->text().trimmed() : ""),
            ui.sshPort->value(),
            (ui.sshKeysGroup->isChecked() ? ui.privateKeyPath->text().trimmed() : ""));
    }

    if (isSslUsed()) {
        conf.setSsl(true);
        conf.setSslSettigns(ui.sslCACertEdit->text().trimmed(),
                            ui.sslPrivateKeyEdit->text().trimmed(),
                            ui.sslLocalCertEdit->text().trimmed());
    }

    return conf;
}
