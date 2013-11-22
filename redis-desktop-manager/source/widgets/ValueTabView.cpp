#include "ValueTabView.h"
#include "FastItemDelegate.h"

void ValueTabView::init(QWidget * baseController)
{
	controller = baseController;

	initLayout();

	initKeyName();

	singleValueFormatterType = new QComboBox;
	singleValueFormatterType->insertItem(0, "Plain text");
	singleValueFormatterType->insertItem(1, "JSON");
	singleValueFormatterType->insertItem(2, "PHP-Serializer (experimental)");
	singleValueFormatterType->setCurrentIndex(0);

	formatterLabel = new QLabel;
	formatterLabel->setText("Value formatter:");
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

		singleValue = new QPlainTextEdit;
		singleValue->appendPlainText("Select table cell");
		singleValue->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
		singleValue->setReadOnly(true);


		singleValueGroup = new QGroupBox("View Value:");

		QGridLayout *grid = new QGridLayout;
		grid->addWidget(formatterLabel, 0, 0, 1, 1);	
		grid->addWidget(singleValueFormatterType, 0, 1, 1, 1);	
		grid->addWidget(singleValue, 1, 0, 1, 2);	
		
		singleValueGroup->setLayout(grid);

		splitter = new QSplitter();
		splitter->setOrientation(Qt::Vertical);
		splitter->addWidget(keyValue);
		splitter->addWidget(singleValueGroup);

		gridLayout->addWidget(splitter, 1, 1, 1, 1);

		initPagination();

	} else {

		keyValuePlain = new QPlainTextEdit(controller);
		keyValuePlain->setReadOnly(true);
		gridLayout->addWidget(singleValueFormatterType, 1, 1, 1, 1);
		gridLayout->addWidget(keyValuePlain, 2, 1, 1, 1);
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