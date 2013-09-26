#ifndef CONNECTION_H
#define CONNECTION_H

#include <QDialog>
#include "ui_connection.h"
#include "demo.h"
#include "RedisConnectionConfig.h"

class connection : public QDialog
{
	Q_OBJECT

public:
	connection(QWidget *parent = 0);
	~connection();

private:
	Ui::connectionDialog ui;
	Main * mainForm;

	bool isFormDataValid();
	bool isConnectionSettingsValid();
	bool isSshSettingsValid();
	bool isSshTunnelUsed();

	RedisConnectionConfig getConectionConfigFromFormData();

	private slots:
		void OnOkButtonClick();
};

#endif // CONNECTION_H
