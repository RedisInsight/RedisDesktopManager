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
	connection(QWidget *parent = 0, RedisServerItem * c = 0);
	~connection();

private:
	Ui::connectionDialog ui;
	Main * mainForm;
	RedisServerItem * server;

	bool inEditMode;

	bool isFormDataValid();
	bool isConnectionSettingsValid();
	bool isSshSettingsValid();
	bool isSshTunnelUsed();

	RedisConnectionConfig getConectionConfigFromFormData();

	void loadValuesFromConnection(RedisConnectionAbstract *);

	private slots:
		void OnOkButtonClick();
};

#endif // CONNECTION_H
