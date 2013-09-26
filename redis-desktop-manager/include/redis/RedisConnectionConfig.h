#pragma once

#include <QtCore>
#include <QtXml>

#define DEFAULT_REDIS_PORT 6379
#define DEFAULT_SSH_PORT 22

class RedisConnectionConfig
{
public:
	// redis connection parameters
	QString name;
	const QString host;
	QString auth;
	const int port;

	// ssh tunnel connection parameters
	QString sshHost;
	int sshPort;
	QString sshUser;
	QString sshPassword;

	//timeouts 
	static const int connectionTimeout = 30000;
	static const int executeTimeout = 30000;

	RedisConnectionConfig(const QString & host, const QString & name = "", const int port = DEFAULT_REDIS_PORT) 
		: host(host), port(port), name(name), sshPort(DEFAULT_SSH_PORT)
	{};

	void setSshTunnelSettings(QString host, QString user, QString pass, int port = DEFAULT_SSH_PORT);
	
	bool isNull();
	bool useSshTunnel() const;
	bool useAuth() const;

	const QString& getName();
	
	QDomElement toXml(QDomDocument dom);	

	static RedisConnectionConfig createFromXml(QDomNode & connectionNode);
};

