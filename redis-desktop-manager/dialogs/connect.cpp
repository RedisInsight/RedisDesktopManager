#include "connect.h"

#include <QtWidgets/QMessageBox>
#include <QFileDialog>
#include <QFile>
#include "application.h"
#include "RedisServerItem.h"
#include "connection.h"
#include "connectionconfig.h"
#include "connectionsmanager.h"

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
    connect(ui.okButton, SIGNAL(pressed()), ui.okButton, SIGNAL(clicked()));
    connect(ui.showPasswordCheckbox, SIGNAL(stateChanged(int)), this, SLOT(OnShowPasswordCheckboxChanged(int)));
    connect(ui.selectPrivateKeyPath, SIGNAL(clicked()), this,  SLOT(OnBrowseSshKeyClick()));
    connect(ui.testConnectionButton, SIGNAL(clicked()), this, SLOT(OnTestConnectionButtonClick()));

    //edit mode
    if (srv != nullptr) {    
        server = srv;
        loadValuesFromConnection(srv->getConnection());
    } else {
        ui.namespaceSeparator->setText(QString(RedisClient::ConnectionConfig::DEFAULT_NAMESPACE_SEPARATOR));
        ui.connectionTimeout->setValue(DEFAULT_TIMEOUT_IN_MS / 1000);
        ui.executionTimeout->setValue(DEFAULT_TIMEOUT_IN_MS / 1000);
    }
}

void ConnectionWindow::loadValuesFromConnection(RedisClient::Connection * c)
{
    inEditMode = true;

    RedisClient::ConnectionConfig config = c->getConfig();

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

        ui.sshKeysGroup->setChecked(false);
        ui.sshPasswordGroup->setChecked(false);

        if (!ui.sshPass->text().isEmpty()) {
            ui.sshPasswordGroup->setChecked(true);
        }

        if (!ui.privateKeyPath->text().isEmpty()) {
            ui.sshKeysGroup->setChecked(true);
        }
    }
}

void ConnectionWindow::OnOkButtonClick()
{
    ui.validationWarning->hide();

    if (!isFormDataValid() || mainForm->connections == nullptr) { 
        ui.validationWarning->show();
        return;    
    }

    RedisClient::ConnectionConfig conf = getConectionConfigFromFormData();

    RedisClient::Connection * connection;

    if (inEditMode) {
        connection = server->getConnection();
        connection->setConnectionConfig(conf);    
        mainForm->connections->connectionChanged();
        
    } else {        
        connection = new RedisClient::Connection(conf, false);
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

void ConnectionWindow::OnBrowseSshKeyClick()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select private key file", "", tr("All Files (*.*)"));

    if (fileName.isEmpty()) {
        return;
    }

    ui.privateKeyPath->setText(fileName);
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
    config.connectionTimeout = 8000;

    RedisClient::Connection testConnection(config, false);

    if (testConnection.connect()) {
        QMessageBox::information(this, "Successful connection", "Successful connection to redis-server");
    } else {
        QMessageBox::warning(this, "Can't connect to redis-server", "Can't connect to redis-server");
    }

    ui.testConnectionButton->setIcon(QIcon());
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
    ui.privateKeyPath->setStyleSheet("");

    if (!isSshTunnelUsed()) {
        return true;
    }

    bool isValid =  !ui.sshHost->text().isEmpty()  
            && !ui.sshUser->text().isEmpty() 
            && !(ui.sshPass->text().isEmpty() && ui.privateKeyPath->text().isEmpty())
            && (
                (ui.sshPasswordGroup->isChecked() && !ui.sshPass->text().isEmpty())
                ||
                (ui.sshKeysGroup->isChecked() && !ui.privateKeyPath->text().isEmpty() && QFile::exists(ui.privateKeyPath->text()))
                )
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

    if (ui.sshPasswordGroup->isChecked() && ui.sshPass->text().isEmpty()) {
        ui.sshPass->setStyleSheet("border: 1px solid red;");
    }

    if (ui.sshKeysGroup->isChecked() && ui.privateKeyPath->text().isEmpty()) {
        ui.privateKeyPath->setStyleSheet("border: 1px solid red;");
    }

    if (!ui.sshPasswordGroup->isChecked() && !ui.sshKeysGroup->isChecked()) {
        ui.sshPass->setStyleSheet("border: 1px solid red;");
        ui.privateKeyPath->setStyleSheet("border: 1px solid red;");
    }


    return false;
}

bool ConnectionWindow::isSshTunnelUsed()
{
    return ui.useSshTunnel->checkState() == Qt::Checked;
}

RedisClient::ConnectionConfig ConnectionWindow::getConectionConfigFromFormData()
{    
    RedisClient::ConnectionConfig conf(ui.hostEdit->text().trimmed(),ui.nameEdit->text().trimmed(), ui.portSpinBox->value());

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
            (ui.sshPasswordGroup->isChecked()? ui.sshPass->text().trimmed() : ""),
            ui.sshPort->value(),
            (ui.sshKeysGroup->isChecked() ? ui.privateKeyPath->text().trimmed() : "")
            );
    }

    return conf;
}

