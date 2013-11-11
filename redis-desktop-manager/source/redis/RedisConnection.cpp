#include "RedisConnection.h"
#include "Command.h"


RedisConnection::RedisConnection(const RedisConnectionConfig & c) 
	: RedisConnectionAbstract(c)
{	
}

RedisConnection::~RedisConnection()
{
	if (connected) {
		socket->disconnectFromHost();
	}

	delete socket;
}


bool RedisConnection::connect()
{
	if (socket == nullptr) {
		socket = new QTcpSocket();

		QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
		QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));	
	}

	socket->connectToHost(config.host, config.port);

	if (socket->waitForConnected(config.connectionTimeout)) 
	{
		connected = true;

		if (config.useAuth()) {
			execute(QString("AUTH %1").arg(config.auth));
		}
	} else {
		connected = false;
	}

	return connected;
}


QString RedisConnection::getLastError()
{
	return socket->errorString();
}

QVariant RedisConnection::execute(QString command)
{		
	if (command.isEmpty()) {
		return QVariant();
	}

	QString formattedCommand = Command::getFormatted(command);

	/*
	 *	Send command
	 */
	QTextStream out(socket);
	out << formattedCommand;
	out.flush();

	if (!socket->waitForReadyRead(config.executeTimeout)) {

		QAbstractSocket::SocketError error = socket->error();

		if (error == QAbstractSocket::UnknownSocketError && connect()) {
			return execute(command);
		}

		return QVariant();
	}

	/*
	 *	Get response
	 */	
	Response response; QByteArray res;

	while(!response.isValid()) {	

		if (socket->bytesAvailable() > 0) 
		{
			res = socket->readAll();
			response.appendToSource(res);	

		} else {
			
			// TODO: move config.executeTimeout to config options - user probably want to increase this value for unstable connections			

			if (!socket->waitForReadyRead(config.executeTimeout)) 
			{
				break;
			}
		}

	}	

	return response.getValue();
}


void RedisConnection::runCommand(const Command &command)
{
	if (command.hasDbIndex()) {
		selectDb(command.getDbIndex());
	}

	resp.clear();
	commandRunning = true;
	runningCommand = command;
	executionTimer.start(config.executeTimeout);

	if (command.isEmpty()) {
		return sendResponse();
	}

	QString formattedCommand = command.getFormattedString();

	/*
	 *	Send command
	 */
	QTextStream out(socket);
	out << formattedCommand;
	out.flush();
}

void RedisConnection::readyRead()
{
	// ignore signals if running blocking version
	if (!commandRunning || socket->bytesAvailable() <= 0) {
		return;
	}
	
	executionTimer.stop();
	readingBuffer = socket->readAll();
	resp.appendToSource(readingBuffer);		

	if (resp.isValid()) {
		return sendResponse();	
	} else {
		executionTimer.start(config.executeTimeout); //restart execution timer
	}
}

void RedisConnection::error(QAbstractSocket::SocketError error)
{
	// ignore signals if running blocking version
	if (!commandRunning) {
		return;
	}

	if (error == QAbstractSocket::UnknownSocketError && connect()) {
		return runCommand(runningCommand);
	}

	return sendResponse();	
}