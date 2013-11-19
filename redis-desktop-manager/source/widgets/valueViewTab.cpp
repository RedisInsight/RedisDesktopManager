#include "valueViewTab.h"
#include "HashKeyModel.h"
#include "RedisKeyItem.h"
#include "ListKeyModel.h"
#include "SortedSetKeyModel.h"
#include "ValueTabView.h"

ValueTab::ValueTab(RedisKeyItem * key)	
	: keyModel(key->getKeyModel()), ui(nullptr), model(nullptr)
{	
	ui = new ValueTabView();

	init();

	connect(keyModel, SIGNAL(valueLoaded(const QVariant&, QObject *)), this, SLOT(valueLoaded(const QVariant&, QObject *)));

	keyModel->getValue();
}

void ValueTab::init()
{
	type = keyModel->getKeyType();		

	if (type == KeyModel::String) {
		ui->init(this, ValueTabView::PlainBased);
	} else {
		ui->init(this);
	}	

	ui->keyName->setText(keyModel->getKeyName());	
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
	}

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

	if (pagesCount > 1) {
		ui->pagination->setText(QString("Page <b>1</b> of <b>%1</b>").arg(pagesCount));
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

ValueTab::~ValueTab()
{
	if (ui != nullptr) {
		delete ui;
	}

	if (model != nullptr) {
		delete model;
	}
}




