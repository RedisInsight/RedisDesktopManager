#pragma once

#include <QtCore/QObject>
#include "qxtsshtcpsocket.h"
#include "qxtsshclient.h"
#include "RedisConnectionAbstract.h"

class RedisConnectionOverSsh : public QObject, public RedisConnectionAbstract
{
	Q_OBJECT

public:
	RedisConnectionOverSsh(const RedisConnectionConfig &);
	~RedisConnectionOverSsh();

	bool connect();
	QString getLastError();
	QVariant execute(QString);

	bool isConnected()
	{
		return connected && socketConnected;
	}

private: 
	QxtSshTcpSocket * socket;
	QxtSshClient sshClient;
	bool isHostKeyAlreadyAdded;
	QEventLoop syncLoop;
	QTimer syncTimer;
	bool socketConnected;

	protected slots:
		void OnSshConnectionError(QxtSshClient::Error);
		void OnSshConnected();
		void OnSocketReadyRead();
		void OnAuthRequired(QList<QxtSshClient::AuthenticationMethod>);

};

