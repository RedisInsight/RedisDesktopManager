#include "connection.h"

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

    if (parent) {
        mainForm = qobject_cast<MainWin *>(parent);
    }

    // connect slots to signals
    connect(ui.okButton, SIGNAL(clicked()), this, SLOT(OnOkButtonClick()));
    connect(ui.okButton, SIGNAL(pressed()), this,  SLOT(clicked()));

    //edit mode
    if (srv != nullptr) {    
        server = srv;
        loadValuesFromConnection(srv->getConnection());
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

    if (config.useSshTunnel()) {
        ui.useSshTunnel->setCheckState(Qt::Checked);
        ui.sshHost->setText(config.sshHost);
        ui.sshUser->setText(config.sshUser);
        ui.sshPass->setText(config.sshPassword);
        ui.sshPort->setValue(config.sshPort);
    }
}

void ConnectionWindow::OnOkButtonClick()
{
    if (!isFormDataValid()) 
        return;    

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

bool ConnectionWindow::isFormDataValid()
{    
    return isConnectionSettingsValid() && isSshSettingsValid();
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

    if (!ui.authEdit->text().isEmpty()) {
        conf.auth = ui.authEdit->text();
    }

    if (isSshTunnelUsed()) {
        conf.setSshTunnelSettings(
            ui.sshHost->text().trimmed(), 
            ui.sshUser->text().trimmed(), 
            ui.sshPass->text().trimmed(), 
            ui.sshPort->value()
            );
    }

    return conf;
}
