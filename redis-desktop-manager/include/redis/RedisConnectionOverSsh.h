#pragma once

#include <QtCore/QObject>
#include "qxtsshtcpsocket.h"
#include "qxtsshclient.h"
#include "RedisConnectionAbstract.h"

class Command;

class RedisConnectionOverSsh : public RedisConnectionAbstract
{
	Q_OBJECT

public:
	RedisConnectionOverSsh(const RedisConnectionConfig &);
	~RedisConnectionOverSsh();

	bool connect();

	QString getLastError();

	QVariant execute(QString);

	void runCommand(const Command&);

	bool isConnected()
	{
		return connected && socketConnected;
	}

public slots:
	void disconnect();

protected:

	void init();

private: 
	QxtSshTcpSocket * socket;
	QxtSshClient * sshClient;
	bool isHostKeyAlreadyAdded;
	QEventLoop * syncLoop;
	QTimer * syncTimer;
	bool socketConnected;

	bool waitForData(int ms);	

	protected slots:
		void OnSshConnectionError(QxtSshClient::Error);
		void OnSshConnected();
		void OnSocketReadyRead();
};

