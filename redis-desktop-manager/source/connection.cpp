#include <QtWidgets/QMessageBox>

#include "connection.h"
#include "RedisConnection.h"
#include "RedisConnectionOverSsh.h"
#include "RedisServerItem.h"

connection::connection(QWidget *parent, RedisServerItem * srv)
	: QDialog(parent), inEditMode(false)
{
	ui.setupUi(this);

	this->setWindowFlags(Qt::Tool);		
	this->setModal(true);

	if (parent) {
		mainForm = qobject_cast<Main *>(parent);
	}

	// connect slots to signals
	connect(ui.okButton, SIGNAL(clicked()), SLOT(OnOkButtonClick()));

	//edit mode
	if (srv != nullptr) {	
		server = srv;
		loadValuesFromConnection(srv->getConnection());
	}
}

connection::~connection()
{

}

void connection::loadValuesFromConnection(RedisConnectionAbstract * c)
{
	inEditMode = true;

	RedisConnectionConfig * config =  &(c->config);

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

	RedisConnectionAbstract * connection;

	if (inEditMode) {

		connection = server->getConnection();

		bool connectionTypeChanged = connection->config.useSshTunnel() != conf.useSshTunnel();

		connection->config = conf;	

		if (connectionTypeChanged) {

			RedisConnectionAbstract * newConnection;

			if (conf.useSshTunnel()) {
				newConnection = new RedisConnectionOverSsh(conf);
			} else {
				newConnection = new RedisConnection(conf);
			}
			
			server->setConnection(newConnection);

			mainForm->connections->UpdateConnection(connection, newConnection);
		}		
		
	} else {
		 
		if (conf.useSshTunnel()) {
			connection = new RedisConnectionOverSsh(conf);
		} else {
			connection = new RedisConnection(conf);
		}

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
