#include "ValueTabView.h"
#include "FastItemDelegate.h"

void ValueTabView::init(QWidget * baseController)
{
	controller = baseController;

	initLayout();

	initKeyName();

	initKeyValue();
}

void ValueTabView::initLayout()
{
	controller->resize(513, 313);
	gridLayout = new QGridLayout(controller);
	gridLayout->setSpacing(6);
	gridLayout->setContentsMargins(11, 11, 11, 11);
	gridLayout->setObjectName(QStringLiteral("gridLayout"));

	loader = new QMovie(":/images/loader.gif");
	loaderLabel = new QLabel;
	gridLayout->addWidget(loaderLabel, 0, 3, 1, 1);
	loaderLabel->setMovie(loader);
	loader->start();
}

void ValueTabView::initKeyName()
{
	keyName = new QLineEdit(controller);
	keyName->setObjectName(QStringLiteral("keyName"));
	keyName->setEnabled(true);
	keyName->setReadOnly(true);

	gridLayout->addWidget(keyName, 0, 1, 1, 1);

	keyNameLabel = new QLabel(controller);
	keyNameLabel->setText("Key:");

	gridLayout->addWidget(keyNameLabel, 0, 0, 1, 1);
}

void ValueTabView::initKeyValue(ValueTabView::Type t)
{
	if (t == ModelBased) {

		keyValue = new QTableView(controller);
		keyValue->setEditTriggers(QAbstractItemView::NoEditTriggers);
		keyValue->setWordWrap(true);
		keyValue->horizontalHeader()->setDefaultSectionSize(220);
		gridLayout->addWidget(keyValue, 1, 1, 1, 1);

		initPagination();

	} else {

		keyValuePlain = new QPlainTextEdit(controller);
		keyValuePlain->setReadOnly(true);
		gridLayout->addWidget(keyValuePlain, 1, 1, 1, 1);
	}

	keyValueLabel = new QLabel(controller);
	keyValueLabel->setText("Value:");
	gridLayout->addWidget(keyValueLabel, 1, 0, 1, 1);
}

void ValueTabView::initPagination()
{
	paginationGrid = new QGridLayout();

	previousPage = new QPushButton();
	previousPage->setText("<");
	previousPage->setEnabled(false);

	nextPage = new QPushButton();
	nextPage->setText(">");
	nextPage->setEnabled(false);

	pagination = new QLabel;
	pagination->setAlignment(Qt::AlignCenter);

	paginationGrid->addWidget(previousPage, 0, 0, 1, 1);
	paginationGrid->addWidget(pagination, 0, 1, 1, 1);
	paginationGrid->addWidget(nextPage, 0, 2, 1, 1);

	gridLayout->addLayout(paginationGrid, 3, 1, 1, 1);
}


void ValueTabView::setModel(QAbstractItemModel * model)
{
	if (keyValue == nullptr) {
		return;
	}

	keyValue->setItemDelegate(new FastItemDelegate);
	keyValue->setModel(model);
}

void ValueTabView::setPlainValue(QString &value)
{
	if (keyValuePlain == nullptr) {
		return;
	}

	keyValuePlain->setPlainText(value);
}