#include "connect.h"

#include <QtWidgets/QMessageBox>

#include "application.h"
#include "ConnectionBridge.h"
#include "RedisServerItem.h"
#include "RedisConnectionConfig.h"
#include "RedisConnectionsManager.h"

ConnectionWindow::ConnectionWindow(QWidget *parent, RedisServerItem * srv)
    : QDialog(parent), inEditMode(false)
{
    ui.setupUi(this);

    this->setWindowFlags(Qt::Tool);        
    this->setModal(true);

    ui.validationWarning->hide();

    if (parent) {
        mainForm = qobject_cast<MainWin *>(parent);
    }

    // connect slots to signals
    connect(ui.okButton, SIGNAL(clicked()), this, SLOT(OnOkButtonClick()));
    connect(ui.okButton, SIGNAL(pressed()), ui.okButton, SIGNAL(clicked()));
    connect(ui.showPasswordCheckbox, SIGNAL(stateChanged(int)), this,  SLOT(OnShowPasswordCheckboxChanged(int)));    

    //edit mode
    if (srv != nullptr) {    
        server = srv;
        loadValuesFromConnection(srv->getConnection());
    } else {
        ui.namespaceSeparator->setText(QString(RedisConnectionConfig::DEFAULT_NAMESPACE_SEPARATOR));
        ui.connectionTimeout->setValue(DEFAULT_TIMEOUT_IN_MS / 1000);
        ui.executionTimeout->setValue(DEFAULT_TIMEOUT_IN_MS / 1000);
    }
}

void ConnectionWindow::loadValuesFromConnection(ConnectionBridge * c)
{
    inEditMode = true;

    RedisConnectionConfig config = c->getConfig();

    ui.nameEdit->setText(config.name);
    ui.hostEdit->setText(config.host);
    ui.portSpinBox->setValue(config.port);
    ui.authEdit->setText(config.auth);
    ui.namespaceSeparator->setText(config.namespaceSeparator);
    ui.connectionTimeout->setValue(config.connectionTimeout / 1000);
    ui.executionTimeout->setValue(config.executeTimeout / 1000);

    if (config.useSshTunnel()) {
        ui.useSshTunnel->setCheckState(Qt::Checked);
        ui.sshHost->setText(config.sshHost);
        ui.sshUser->setText(config.sshUser);
        ui.sshPass->setText(config.sshPassword);
        ui.sshPort->setValue(config.sshPort);
        ui.privateKeyPath->setText(config.sshPrivateKeyPath);
    }
}

void ConnectionWindow::OnOkButtonClick()
{
    ui.validationWarning->hide();

    if (!isFormDataValid() || mainForm->connections == nullptr) { 
        ui.validationWarning->show();
        return;    
    }

    RedisConnectionConfig conf = getConectionConfigFromFormData();

    ConnectionBridge * connection;

    if (inEditMode) {
        connection = server->getConnection();
        connection->setConnectionConfig(conf);    
        mainForm->connections->connectionChanged();
        
    } else {        
        connection = new ConnectionBridge(conf);
        mainForm->connections->AddConnection(connection);            
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

bool ConnectionWindow::isFormDataValid()
{    
    return isConnectionSettingsValid() 
        && isSshSettingsValid()
        && isAdvancedSettingsValid();
}

bool ConnectionWindow::isConnectionSettingsValid()
{
    ui.nameEdit->setStyleSheet("");
    ui.hostEdit->setStyleSheet("");

    bool isValid = !ui.nameEdit->text().isEmpty()
        && !ui.hostEdit->text().isEmpty()
        && ui.portSpinBox->value() > 0;

    if (isValid) {
        return true;
    } 

    if (ui.nameEdit->text().isEmpty()) {
        ui.nameEdit->setStyleSheet("border: 1px solid red;");
    }

    if (ui.hostEdit->text().isEmpty()) {
        ui.hostEdit->setStyleSheet("border: 1px solid red;");
    }

    return false;
}

bool ConnectionWindow::isAdvancedSettingsValid()
{
    ui.namespaceSeparator->setStyleSheet("");

    bool isValid = !ui.namespaceSeparator->text().isEmpty();

    if (isValid) {
        return true;
    } 

    if (ui.namespaceSeparator->text().isEmpty()) {
        ui.namespaceSeparator->setStyleSheet("border: 1px solid red;");
    }

    return false;
}

bool ConnectionWindow::isSshSettingsValid()
{
    ui.sshHost->setStyleSheet("");
    ui.sshUser->setStyleSheet("");
    ui.sshPass->setStyleSheet("");

    if (!isSshTunnelUsed()) {
        return true;
    }

    bool isValid =  !ui.sshHost->text().isEmpty()  
            && !ui.sshUser->text().isEmpty() 
            && !ui.sshPass->text().isEmpty() 
            && ui.sshPort->value() > 0;

    if (isValid) {
        return true;
    }

    if (ui.sshHost->text().isEmpty()) {
        ui.sshHost->setStyleSheet("border: 1px solid red;");
    }

    if (ui.sshUser->text().isEmpty()) {
        ui.sshUser->setStyleSheet("border: 1px solid red;");
    }

    if (ui.sshPass->text().isEmpty()) {
        ui.sshPass->setStyleSheet("border: 1px solid red;");
    }

    return false;
}

bool ConnectionWindow::isSshTunnelUsed()
{
    qDebug() << "check state" << (ui.useSshTunnel->checkState() == Qt::Checked);
    return ui.useSshTunnel->checkState() == Qt::Checked;
}

RedisConnectionConfig ConnectionWindow::getConectionConfigFromFormData()
{    
    RedisConnectionConfig conf(ui.hostEdit->text().trimmed(),ui.nameEdit->text().trimmed(), ui.portSpinBox->value());

    conf.namespaceSeparator = ui.namespaceSeparator->text();
    conf.connectionTimeout = ui.connectionTimeout->value() * 1000;
    conf.executeTimeout = ui.executionTimeout->value() * 1000;

    if (!ui.authEdit->text().isEmpty()) {
        conf.auth = ui.authEdit->text();
    }

    if (isSshTunnelUsed()) {
        conf.setSshTunnelSettings(
            ui.sshHost->text().trimmed(), 
            ui.sshUser->text().trimmed(), 
            ui.sshPass->text().trimmed(), 
            ui.sshPort->value(),
            "", ui.privateKeyPath->text().trimmed()
            );
    }

    return conf;
}
