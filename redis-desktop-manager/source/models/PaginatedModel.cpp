#include "PaginatedModel.h"
#include <chrono>
#include <thread>
#include <QtConcurrent>

PaginatedModel::PaginatedModel(ConnectionBridge * db, const QString &keyName, int dbIndex)
	: KeyModel(db, keyName, dbIndex), currentPage(0)
{
}

void PaginatedModel::initModel(const QVariant & val)
{
	rawData = new QVector<QString>;

	if (val.canConvert<QStringList>())
		*rawData = val.value<QStringList>().toVector();
	
	setCurrentPage(1);
}

int PaginatedModel::getCurrentPage()
{
	return currentPage;
}

int PaginatedModel::itemsCount()
{
	return rawData->size();
}

int PaginatedModel::getPagesCount()
{
	int pages = itemsCount() / itemsOnPageLimit;

	if (itemsCount() % itemsOnPageLimit > 0) {
		pages++;
	}

	return pages;
}

PaginatedModel::~PaginatedModel()
{
	if (rawData == nullptr)
		return;

	delete rawData;
}

void PaginatedModel::delayedDeallocator(QObject *object)
{
	QElapsedTimer timer;
	timer.start();

	delete object;

	qDebug() << QString("Async free memory %1").arg(timer.elapsed());
}
