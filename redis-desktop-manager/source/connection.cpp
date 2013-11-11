#include <QtWidgets/QMessageBox>

#include "connection.h"
#include "ConnectionBridge.h"
#include "RedisServerItem.h"

connection::connection(QWidget *parent, RedisServerItem * srv)
	: QDialog(parent), inEditMode(false)
{
	ui.setupUi(this);

	this->setWindowFlags(Qt::Tool);		
	this->setModal(true);

	if (parent) {
		mainForm = qobject_cast<MainWin *>(parent);
	}

	// connect slots to signals
	connect(ui.okButton, SIGNAL(clicked()), SLOT(OnOkButtonClick()));
	connect(ui.okButton, SIGNAL(pressed()), SLOT(clicked()));

	//edit mode
	if (srv != nullptr) {	
		server = srv;
		loadValuesFromConnection(srv->getConnection());
	}
}

connection::~connection()
{

}

void connection::loadValuesFromConnection(ConnectionBridge * c)
{
	inEditMode = true;

	RedisConnectionConfig * config =  &(c->getConfig());

	ui.nameEdit->setText(config->name);
	ui.hostEdit->setText(config->host);
	ui.portSpinBox->setValue(config->port);
	ui.authEdit->setText(config->auth);

	if (config->useSshTunnel()) {
		ui.useSshTunnel->setCheckState(Qt::Checked);
		ui.sshHost->setText(config->sshHost);
		ui.sshUser->setText(config->sshUser);
		ui.sshPass->setText(config->sshPassword);
		ui.sshPort->setValue(config->sshPort);
	}
}

void connection::OnOkButtonClick()
{
	if (!isFormDataValid()) return;	

	RedisConnectionConfig conf = getConectionConfigFromFormData();

	ConnectionBridge * connection;

	if (inEditMode) {

		connection = server->getConnection();

		bool connectionTypeChanged = connection->getConfig().useSshTunnel() != conf.useSshTunnel();

		connection->setConnectionConfig(conf);	
		
	} else {		

		connection = new ConnectionBridge(conf);

		mainForm->connections->AddConnection(connection);			
	}	
	
	close();
}

bool connection::isFormDataValid()
{	
	return isConnectionSettingsValid() && isSshSettingsValid();
}

bool connection::isConnectionSettingsValid()
{
	return !ui.nameEdit->text().isEmpty()
		&& !ui.hostEdit->text().isEmpty()
		&& ui.portSpinBox->value() > 0;
}

bool connection::isSshSettingsValid()
{
	return !isSshTunnelUsed() 
		|| (!ui.sshHost->text().isEmpty()  
			&& !ui.sshUser->text().isEmpty() 
			&& !ui.sshPass->text().isEmpty() 
			&& ui.sshPort->value() > 0);
}

bool connection::isSshTunnelUsed()
{
	return ui.useSshTunnel->checkState() == Qt::Checked;
}

RedisConnectionConfig connection::getConectionConfigFromFormData()
{	
	RedisConnectionConfig conf(ui.hostEdit->text(),ui.nameEdit->text(), ui.portSpinBox->value());

	if (!ui.authEdit->text().isEmpty()) {
		conf.auth = ui.authEdit->text();
	}

	if (isSshTunnelUsed()) {
		conf.setSshTunnelSettings(
			ui.sshHost->text(), 
			ui.sshUser->text(), 
			ui.sshPass->text(), 
			ui.sshPort->value()
			);
	}

	return conf;
}
