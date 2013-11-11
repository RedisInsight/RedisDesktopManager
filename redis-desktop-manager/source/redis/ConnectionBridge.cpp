#include "ConnectionBridge.h"
#include "RedisConnectionAbstract.h"


ConnectionBridge::ConnectionBridge(const RedisConnectionConfig & c)
	: isInitialized(false), config(c)
{
	//create worker connection by factory method
	worker = RedisConnectionAbstract::createConnection(c);
}

void ConnectionBridge::initWorker()
{
	if (isInitialized) 
		return;

	isInitialized = true;

	// move worker to another thread
	workerThread = new QThread;
	worker->moveToThread(workerThread);

	//connect worker & bridge signals and slots
	connect(this, SIGNAL(addCommandToWorker(const Command &)), worker, SLOT(addCommand(const Command&)));
	connect(worker, SIGNAL(responseResived(QVariant &, QObject *)), this, SIGNAL(responseResieved(QVariant &, QObject *)));

	connect(this, SIGNAL(loadDatabasesList()), worker, SLOT(getDatabases()));
	connect(worker, SIGNAL(databesesLoaded(RedisConnectionAbstract::RedisDatabases&)),
		this, SIGNAL(dbListLoaded(RedisConnectionAbstract::RedisDatabases&)));

	//start worker thread
	workerThread->start();
}

void ConnectionBridge::addCommand(const Command & cmd)
{
	if (!isInitialized) {
		initWorker();
	}

	emit addCommandToWorker(cmd);
}

QString ConnectionBridge::getLastError()
{
	return QString(); // todo : fix it
}

void ConnectionBridge::setConnectionConfig(RedisConnectionConfig& newConf)
{
	config = newConf;

	stopWorker();

	delete worker;

	worker = RedisConnectionAbstract::createConnection(config);

	initWorker();
}

RedisConnectionConfig ConnectionBridge::getConfig()
{
	return config;
}

void ConnectionBridge::stopWorker()
{
	if (workerThread->isRunning()) {
		workerThread->quit();
		workerThread->wait();

		isInitialized = false;
	}
}

ConnectionBridge::~ConnectionBridge(void)
{
	stopWorker();

	delete workerThread;
}
