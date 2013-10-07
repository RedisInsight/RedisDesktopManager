#include "RedisConnectionOverSsh.h"

#define MAX_BUFFER_SIZE 536870999 //redis response limit

RedisConnectionOverSsh::RedisConnectionOverSsh(const RedisConnectionConfig &c)
	: RedisConnectionAbstract(c),  socket(nullptr), isHostKeyAlreadyAdded(false), socketConnected(false)
{
	syncTimer.setSingleShot(true);

	QObject::connect(&syncTimer, SIGNAL(timeout()), &syncLoop, SLOT(quit()));
	QObject::connect(&sshClient, SIGNAL(connected()), this, SLOT(OnSshConnected())); 

	QObject::connect(
		&sshClient, SIGNAL(error(QxtSshClient::Error)), 
		this, SLOT(OnSshConnectionError(QxtSshClient::Error))
		);

	QObject::connect(
		&sshClient, SIGNAL(authenticationRequired(QList<QxtSshClient::AuthenticationMethod>)), 
		this, SLOT(OnAuthRequired(QList<QxtSshClient::AuthenticationMethod>))
		);
}

RedisConnectionOverSsh::~RedisConnectionOverSsh(void)
{
	if (socket != nullptr) {
		delete socket;
	}
}

bool RedisConnectionOverSsh::connect() 
{
	//set password
	sshClient.setPassphrase(config.sshPassword);	

	//connect to ssh server
	syncTimer.start(config.connectionTimeout);
	sshClient.connectToHost(config.sshUser, config.sshHost, config.sshPort);
	syncLoop.exec();

	if (!connected && !syncTimer.isActive()) {
		connected = false;
		return connected;
	}

	//connect to redis 
	socket = sshClient.openTcpSocket(config.host, config.port);

	if (socket == NULL) {
		socketConnected = false;
		return socketConnected;
	}

	QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(OnSocketReadyRead())); 

	syncTimer.start(config.connectionTimeout);
	syncLoop.exec();

	if (!socketConnected && !syncTimer.isActive()) {
		socketConnected = false;
		return socketConnected;
	}

	if (config.useAuth()) {
		execute(QString("AUTH %1").arg(config.auth));
	}

	return socketConnected;	
}

void RedisConnectionOverSsh::OnSshConnectionError(QxtSshClient::Error error)
{
	if (QxtSshClient::HostKeyUnknownError == error) {
		QxtSshKey hostKey = sshClient.hostKey();

        sshClient.addKnownHost(config.sshHost, hostKey);

        sshClient.resetState();

		sshClient.connectToHost(config.sshUser, config.sshHost, config.sshPort);

		isHostKeyAlreadyAdded = true;
		return;
	}

	if (syncLoop.isRunning()) {
		syncLoop.exit();
	}

}

void RedisConnectionOverSsh::OnSshConnected()
{
	connected = true;

	if (syncLoop.isRunning()) {
		syncLoop.exit();
	}
}

void RedisConnectionOverSsh::OnSocketReadyRead()
{
	//on first emit
	if (!socketConnected) {
		socketConnected = true;			
	}	

	if (syncLoop.isRunning()) {
		syncLoop.exit();
	}

}

void RedisConnectionOverSsh::OnAuthRequired(QList<QxtSshClient::AuthenticationMethod> authMethods)
{
	int size = authMethods.size();

}

QString RedisConnectionOverSsh::getLastError()
{
	if (socket == NULL) {
		return QString("Error with ssh connection");
	}

	return socket->errorString();
}


QVariant RedisConnectionOverSsh::execute(QString command)
{		
	if (command.isEmpty()) {
		return QVariant();
	}

	QString formattedCommand = prepareCommand(command);

	/*
	 *	Send command
	 */
	QByteArray byteArray = formattedCommand.toUtf8();
	const char* cString = byteArray.constData();

	int result = socket->write(cString, byteArray.size());

	//wait for ready read
	syncTimer.start(config.executeTimeout);
	syncLoop.exec();

	if (!syncTimer.isActive()) {
		return QVariant();
	}

	/*
	 *	Get response
	 */	
	QString response("");		
	bool firstLimit = true, secondLimit = true, thirdLimit = true, lastLimit = true;	

	while(!isFullResponseRecieved(response)) {

		QByteArray availableData = socket->read(MAX_BUFFER_SIZE);

		int currBytesAvailable = availableData.size();

		if (currBytesAvailable > 0) 
		{
			response.append(availableData);	

		} else {						

			if (firstLimit && waitForData(5)) 
			{
				firstLimit = false;
				continue;			
			}

			if (secondLimit && waitForData(50)) 
			{
				secondLimit = false;
				continue;			
			}

			if (thirdLimit && waitForData(100)) 
			{
				thirdLimit = false;
				continue;			
			}

			// TODO: move config.executeTimeout to config options - user probably want to increase this value for unstable connections			
			if (lastLimit && waitForData(config.executeTimeout)) 
			{
				lastLimit = false;
				continue;			
			}

			break;
		}

	}	

	return parseResponse(response);
}
