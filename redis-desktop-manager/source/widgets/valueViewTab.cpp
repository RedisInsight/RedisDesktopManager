#include "valueViewTab.h"
#include "HashKeyModel.h"
#include "RedisKeyItem.h"
#include "ListKeyModel.h"
#include "SortedSetKeyModel.h"
#include "ValueTabView.h"
#include "Command.h"
#include "ConnectionBridge.h"

#include <QtConcurrent>

ValueTab::ValueTab(RedisKeyItem * key)	
	: key(key), ui(nullptr), currentFormatter(AbstractFormatter::FormatterType::Plain), 
	  currentCell(nullptr), formatter(AbstractFormatter::getFormatter())
{	
	ui = new ValueTabView(key->text(), this);	

	Command typeCmd = key->getTypeCommand();
	typeCmd.setOwner(this);
	typeCmd.setCallBackName("keyTypeLoaded");

	key->getConnection()->addCommand(typeCmd);

	/** Connect View SIGNALS to Controller SLOTS **/
	connect(ui->singleValueFormatterType, SIGNAL(currentIndexChanged(int)), 
			this, SLOT(currentFormatterChanged(int)));
	connect(ui->renameKey, SIGNAL(clicked()), this, SLOT(renameKey()));
}

void ValueTab::keyTypeLoaded(const QVariant & type)
{
	QString t = type.toString();
	ui->keyTypeLabelValue->setText(
		ui->keyTypeLabelValue->text()  + t.toUpper()
		);

	keyModel = key->getKeyModel(t);

	if (keyModel == nullptr) {
		emit error("Can not load key value");
		return;
	}

	connect(keyModel, SIGNAL(valueLoaded()), this, SLOT(valueLoaded()));

	keyModel->loadValue();
}

void ValueTab::valueLoaded()
{
	ui->initKeyValue(keyModel);

	setObjectName("valueTabReady");
}

void ValueTab::initPagination()
{
	connect(ui->keyValue->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), 
		this, SLOT(onSelectedItemChanged(const QModelIndex &, const QModelIndex &)));

	PaginatedModel * model = (PaginatedModel *) keyModel;

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
	PaginatedModel * model = (PaginatedModel *) keyModel;

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
	PaginatedModel * model = (PaginatedModel *) keyModel;
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

	formatter->setRawValue(keyModel->itemFromIndex(current)->text());

	ui->singleValue->appendPlainText(formatter->getFormatted());

	currentCell = &current;
}

void ValueTab::currentFormatterChanged(int index)
{
	AbstractFormatter::FormatterType newFormatterType = (AbstractFormatter::FormatterType)index;

	if (newFormatterType == currentFormatter) 
		return;

	currentFormatter = newFormatterType;
	delete formatter;

	formatter = AbstractFormatter::getFormatter(newFormatterType);

	//TODO: fix it

// 	if (type == KeyModel::String) {		
// 		ui->keyValuePlain->clear();
// 		formatter->setRawValue(rawStringValue);
// 		ui->keyValuePlain->appendPlainText(
// 			formatter->getFormatted()
// 		);
// 	} else {
// 		onSelectedItemChanged(*currentCell, *currentCell);
// 	}	
}

void ValueTab::renameKey()
{
	keyModel->renameKey(ui->keyName->text());
}

void ValueTab::deleteKey()
{
	//todo implement this
}

ValueTab::~ValueTab()
{
	if (keyModel != nullptr) {

		keyModel->disconnect();

		QtConcurrent::run(delayedDeallocator, keyModel);

		keyModel = nullptr;		
	}
}

void ValueTab::delayedDeallocator(QObject *object)
{
	delete object;
}



