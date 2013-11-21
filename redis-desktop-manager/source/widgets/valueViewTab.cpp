#include "valueViewTab.h"
#include "HashKeyModel.h"
#include "RedisKeyItem.h"
#include "ListKeyModel.h"
#include "SortedSetKeyModel.h"
#include "ValueTabView.h"

#include <QtConcurrent>
#include <json\json.h>

ValueTab::ValueTab(RedisKeyItem * key)	
	: keyModel(key->getKeyModel()), ui(nullptr), model(nullptr), 
	  currentFormatter(ValueViewFormatters::Plain), currentCell(nullptr)
{	
	ui = new ValueTabView();
	ui->init(this);
	ui->keyName->setText(keyModel->getKeyName());	

	connect(keyModel, SIGNAL(keyTypeLoaded(KeyModel::Type)), this, SLOT(keyTypeLoaded(KeyModel::Type)));
	connect(keyModel, SIGNAL(valueLoaded(const QVariant&, QObject *)), this, SLOT(valueLoaded(const QVariant&, QObject *)));	
	connect(ui->singleValueFormatterType, SIGNAL(currentIndexChanged(int)), 
		this, SLOT(currentFormatterChanged(int)));

	keyModel->getKeyType();	
}

void ValueTab::keyTypeLoaded(KeyModel::Type t)
{
	type = t;

	if (type == KeyModel::String) {
		ui->initKeyValue(ValueTabView::PlainBased);
	} else {
		ui->initKeyValue(ValueTabView::ModelBased);
	}

	keyModel->getValue();
}

void ValueTab::valueLoaded(const QVariant& value, QObject * owner)
{
	if (owner != keyModel) {
		return;
	}

	ui->loader->stop();
	ui->loaderLabel->hide();

	if (type == KeyModel::String) {
        QString rawValue = value.toString();
        ui->setPlainValue(rawValue);
	} else {
		model = getModelForKey(type, value);
		ui->setModel(model);
		initPagination();

		connect(ui->keyValue->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), 
			this, SLOT(onSelectedItemChanged(const QModelIndex &, const QModelIndex &)));
	}

	setObjectName("valueTabReady");
}

PaginatedModel * ValueTab::getModelForKey(KeyModel::Type t, const QVariant& val)
{
    QStringList rawValue = val.toStringList();

    switch (t)
	{
	case KeyModel::Hash:		
        return new HashKeyModel(rawValue);

	case KeyModel::List:		
	case KeyModel::Set:
        return new ListKeyModel(rawValue);

	case KeyModel::ZSet:		
        return new SortedSetKeyModel(rawValue);
	}

	return nullptr;
}


void ValueTab::initPagination()
{
	if (model == nullptr) {
		return;
	}

	int pagesCount = model->getPagesCount();
	ui->pagination->setText(QString("Page <b>1</b> of <b>%1</b>").arg(pagesCount));

	if (pagesCount > 1) {		
		ui->nextPage->setEnabled(true);

		connect(ui->nextPage, SIGNAL(clicked()), this, SLOT(loadNextPage()));
		connect(ui->previousPage, SIGNAL(clicked()), this, SLOT(loadPreviousPage()));
	}

}

void ValueTab::loadNextPage()
{
	int currentPage = model->getCurrentPage();
	int totalPages = model->getPagesCount();

	if (currentPage == totalPages) {
		return;
	}

	model->setCurrentPage(++currentPage);

	if (currentPage == totalPages) {
		ui->nextPage->setEnabled(false);
	}

	if (currentPage == 2) {
		ui->previousPage->setEnabled(true);
	}

	ui->pagination->setText(
		QString("Page <b>%1</b> of <b>%2</b>").arg(currentPage).arg(totalPages));
}

void ValueTab::loadPreviousPage()
{
	int currentPage = model->getCurrentPage();
	int totalPages = model->getPagesCount();

	if (currentPage == 1) {
		return;
	}

	model->setCurrentPage(--currentPage);

	if (currentPage == totalPages - 1) {
		ui->nextPage->setEnabled(true);
	}

	if (currentPage == 1) {
		ui->previousPage->setEnabled(false);
	}

	ui->pagination->setText(
		QString("Page <b>%1</b> of <b>%2</b>").arg(currentPage).arg(totalPages));
}

void ValueTab::onSelectedItemChanged(const QModelIndex & current, const QModelIndex & previous)
{
	ui->singleValue->clear();

	QString value = model->itemFromIndex(current)->text();	

	switch (currentFormatter)
	{
	
	case ValueViewFormatters::Json:
		value = jsonValueFormatter(value);
		break;
	case ValueViewFormatters::PHPSerializer:
		//todo : implement this
		break;
	case ValueViewFormatters::XML:
		//todo : implement this
		break;
	case ValueViewFormatters::Plain:
	default:
		break;		
	}

	ui->singleValue->appendPlainText(value);

	currentCell = &current;
}

void ValueTab::currentFormatterChanged(int index)
{
	currentFormatter = (ValueViewFormatters)index;
	onSelectedItemChanged(*currentCell, *currentCell);
}

QString ValueTab::jsonValueFormatter(const QString& plainValue)
{
	Json::Value root;   
	Json::Reader reader;
	bool parsingSuccessful = reader.parse( plainValue.toStdString(), root );

	if (!parsingSuccessful)
	{
		return QString("Invalid JSON");
	}

	Json::StyledWriter writer;		

	return QString::fromStdString(writer.write(root));
}

ValueTab::~ValueTab()
{
	if (ui != nullptr) {
		delete ui;
	}

	if (model != nullptr) {

		model->disconnect();

		QtConcurrent::run(delayedDeallocator, model);

		model = nullptr;		
	}

	keyModel->disconnect(this);
}


void ValueTab::delayedDeallocator(QObject *object)
{
	delete object;
}



