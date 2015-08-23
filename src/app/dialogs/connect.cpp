#include "connect.h"

#include <QtWidgets/QMessageBox>
#include <QFileDialog>
#include <QFile>
#include "redisclient/connection.h"
#include "redisclient/connectionconfig.h"
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
        OnBrowseFileClick(ui.sslCACertEdit, "Select PEM file", tr("All Files (*.pem)"));
    });
    connect(ui.sslLocalCertButton, &QPushButton::clicked, this, [this]() {
        OnBrowseFileClick(ui.sslLocalCertEdit, "Select PEM file", tr("All Files (*.pem)"));
    });
    connect(ui.sslPrivateKeyButton, &QPushButton::clicked, this, [this]() {
        OnBrowseFileClick(ui.sslPrivateKeyEdit, "Select PEM file", tr("All Files (*.pem)"));
    });
    connect(ui.testConnectionButton, SIGNAL(clicked()), this, SLOT(OnTestConnectionButtonClick()));
    connect(ui.showPasswordCheckbox, SIGNAL(stateChanged(int)), this, SLOT(OnShowPasswordCheckboxChanged(int)));

    ui.namespaceSeparator->setText(QString(RedisClient::ConnectionConfig::DEFAULT_NAMESPACE_SEPARATOR));
    ui.keysPattern->setText(QString(RedisClient::ConnectionConfig::DEFAULT_KEYS_GLOB_PATTERN));
    ui.connectionTimeout->setValue(DEFAULT_TIMEOUT_IN_MS / 1000);
    ui.executionTimeout->setValue(DEFAULT_TIMEOUT_IN_MS / 1000);
}

void ConnectionWindow::setConnectionConfig(const RedisClient::ConnectionConfig& config)
{
    m_config = config;
    m_inEditMode = true;
    loadValuesFromConfig(m_config);
}

void ConnectionWindow::loadValuesFromConfig(const RedisClient::ConnectionConfig& config)
{
    m_inEditMode = true;

    ui.nameEdit->setText(config.name());
    ui.hostEdit->setText(config.host());
    ui.portSpinBox->setValue(config.port());
    ui.authEdit->setText(config.auth());
    ui.namespaceSeparator->setText(config.param<QString>("namespace_separator"));
    ui.connectionTimeout->setValue(config.connectionTimeout()/1000);
    ui.executionTimeout->setValue(config.executeTimeout()/1000);

    if (!config.keysPattern().isEmpty()) {
        ui.keysPattern->setText(config.keysPattern());
    }

    if (config.useSshTunnel()) {
        ui.useSshTunnel->setCheckState(Qt::Checked);
        ui.sshHost->setText(config.param<QString>("ssh_host"));
        ui.sshUser->setText(config.param<QString>("ssh_user"));
        ui.sshPass->setText(config.param<QString>("ssh_password"));
        ui.sshPort->setValue(config.param<int>("ssh_port"));
        ui.privateKeyPath->setText(config.param<QString>("ssh_private_key_path"));

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
        ui.sslCACertEdit->setText(config.param<QString>("ssl_ca_cert_path"));
        ui.sslPrivateKeyEdit->setText(config.param<QString>("ssl_private_key_path"));
        ui.sslLocalCertEdit->setText(config.param<QString>("ssl_local_cert_path"));
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

    RedisClient::ConnectionConfig conf = getConectionConfigFromFormData();

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

    RedisClient::ConnectionConfig config = getConectionConfigFromFormData();
    config.setParam("timeout_connect", 8000);

    RedisClient::Connection testConnection(config, false);

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

    markFieldValid(ui.sslCACertEdit);

    bool isValid = !ui.sslCACertEdit->text().isEmpty() &&
            QFile::exists(ui.sslCACertEdit->text().trimmed());

    if (isValid)
        return true;

    markFieldInvalid(ui.sslCACertEdit);

    return false;
}

RedisClient::ConnectionConfig ConnectionWindow::getConectionConfigFromFormData()
{    
    RedisClient::ConnectionConfig conf(ui.hostEdit->text().trimmed(),
                                       ui.nameEdit->text().trimmed(),
                                       ui.portSpinBox->value());

    conf.setParam("namespace_separator", ui.namespaceSeparator->text());
    conf.setParam("timeout_connect", ui.connectionTimeout->value() * 1000);
    conf.setParam("timeout_execute", ui.executionTimeout->value() * 1000);

    if (!ui.authEdit->text().isEmpty())
        conf.setParam("auth", ui.authEdit->text());    

    if (!ui.keysPattern->text().isEmpty())
        conf.setParam("keys_pattern", ui.keysPattern->text());

    if (isSshTunnelUsed()) {
        conf.setSshTunnelSettings(
            ui.sshHost->text().trimmed(), 
            ui.sshUser->text().trimmed(), 
            (ui.sshPasswordGroup->isChecked()? ui.sshPass->text().trimmed() : ""),
            ui.sshPort->value(),
            (ui.sshKeysGroup->isChecked() ? ui.privateKeyPath->text().trimmed() : ""));
    }

    if (isSslUsed()) {
        conf.setParam("ssl_ca_cert_path", ui.sslCACertEdit->text().trimmed());
        conf.setParam("ssl_private_key_path", ui.sslPrivateKeyEdit->text().trimmed());
        conf.setParam("ssl_local_cert_path", ui.sslLocalCertEdit->text().trimmed());
    }

    return conf;
}
