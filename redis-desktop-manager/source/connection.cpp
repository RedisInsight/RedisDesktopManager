#include <QtWidgets/QMessageBox>

#include "connection.h"
#include "RedisConnection.h"
#include "RedisConnectionOverSsh.h"

connection::connection(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	if (parent) {
		mainForm = qobject_cast<Main *>(parent);
	}

	// connect slots to signals
	connect(ui.okButton, SIGNAL(clicked()), SLOT(OnOkButtonClick()));
}

connection::~connection()
{

}

void connection::OnOkButtonClick()
{
	if (!isFormDataValid()) return;	

	RedisConnectionConfig conf = getConectionConfigFromFormData();

	RedisConnectionAbstract * connection = nullptr;

	if (conf.useSshTunnel()) {
		connection = new RedisConnectionOverSsh(conf);
	} else {
		connection = new RedisConnection(conf);
	}

	mainForm->connections->AddConnection(connection);
	
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
