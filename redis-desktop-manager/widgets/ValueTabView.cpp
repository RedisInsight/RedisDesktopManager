#include "ValueTabView.h"
#include "FastItemDelegate.h"
#include "KeyModel.h"
#include "StringKeyModel.h"
#include "PaginatedModel.h"
#include "AbstractFormatter.h"
#include <QMessageBox>

ValueTabView::ValueTabView(const QString& name, QWidget * parent)
    : controller(parent), formatter(AbstractFormatter::getFormatter()),
      currentCell(nullptr)
{    
    initLayout();

    initKeyName();
    keyName->setText(name);

    initFormatter();
}

void ValueTabView::initFormatter()
{
    singleValueFormatterType = new QComboBox;
    singleValueFormatterType->insertItem(0, "Plain text");
    singleValueFormatterType->insertItem(1, "JSON");
    singleValueFormatterType->setCurrentIndex(0);

    formatterLabel = new QLabel;
    formatterLabel->setText("View value as:");

    connect(singleValueFormatterType, SIGNAL(currentIndexChanged(int)), 
        this, SLOT(currentFormatterChanged(int)));
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
    keyNameLabel = new QLabel(controller);
    keyNameLabel->setText(" Key:");
    gridLayout->addWidget(keyNameLabel, 0, 0, 1, 1);

    keyName = new QLineEdit(controller);        

    keyTypeLabelValue = new QLabel(controller);
    keyTypeLabelValue->setText("Type: ");

    keyExpireLabel = new QLabel(controller);
    keyExpireLabel->setText("");

    renameKey = new QPushButton;
    renameKey->setText("Rename");    

    deleteKey = new QPushButton;
    deleteKey->setText("Delete");
    deleteKey->setStyleSheet("color: darkred; font-weight: 800;");

    renameKey->setEnabled(false);
    deleteKey->setEnabled(false);
    keyName->setEnabled(false);

    QGridLayout *grid = new QGridLayout;
    
    grid->addWidget(keyName, 0, 1, 1, 1);    
    grid->addWidget(keyTypeLabelValue, 0, 2, 1, 1);    
    grid->addWidget(keyExpireLabel, 0, 3, 1, 1);  
    grid->addWidget(renameKey, 0, 4, 1, 1);    
    grid->addWidget(deleteKey, 0, 5, 1, 1);    
    gridLayout->addLayout(grid, 0, 1, 1, 1);
}

void ValueTabView::setKeyExpire(int ttl)
{
    if (ttl > 0) 
        keyExpireLabel->setText(QString("TTL: %1 sec").arg(ttl));
    else
        keyExpireLabel->setText("TTL: -");
}

void ValueTabView::initKeyValue(KeyModel * model)
{
    loader->stop();
    loaderLabel->hide();

    this->model = model;

    saveValue = new QPushButton;
    saveValue->setText("Save value");
    saveValue->setStyleSheet("color: green; font-weight: 800;");
    saveValue->setFixedHeight(35);
    saveValue->setFixedWidth(150);
    connect(saveValue, SIGNAL(clicked()), this, SLOT(valueUpdate()));

    renameKey->setEnabled(true);
    deleteKey->setEnabled(true);
    keyName->setEnabled(true);

    singleValue = new QPlainTextEdit(controller);

    if (model->getKeyModelType() == KeyModel::KEY_MODEL_TYPE) {

        keyValue = new QTableView(controller);
        keyValue->setEditTriggers(QAbstractItemView::NoEditTriggers);
        keyValue->setWordWrap(true);
        keyValue->horizontalHeader()->setDefaultSectionSize(220);

        
        singleValue->appendPlainText("Select table cell");
        singleValue->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        singleValueGroup = new QGroupBox("Value:");

        QGridLayout *grid = new QGridLayout;
        grid->addWidget(formatterLabel, 0, 0, 1, 1);    
        grid->addWidget(singleValueFormatterType, 0, 1, 1, 1);    
        grid->addWidget(singleValue, 1, 0, 1, 2);    
        grid->addWidget(saveValue, 2, 1, 1, 1);    
        
        singleValueGroup->setLayout(grid);

        splitter = new QSplitter();
        splitter->setOrientation(Qt::Vertical);
        splitter->addWidget(keyValue);
        splitter->addWidget(singleValueGroup);

        gridLayout->addWidget(splitter, 1, 0, 1, 2);

        keyValue->setItemDelegate(new FastItemDelegate);
        keyValue->setModel(model);

        initPagination();

    } else if (model->getKeyModelType() == StringKeyModel::KEY_MODEL_TYPE) {
            
        gridLayout->addWidget(singleValueFormatterType, 1, 1, 1, 1);
        gridLayout->addWidget(singleValue, 2, 1, 1, 1);
        gridLayout->addWidget(saveValue, 3, 1, 1, 1);

        keyValueLabel = new QLabel(controller);
        keyValueLabel->setText("Value:");
        gridLayout->addWidget(keyValueLabel, 1, 0, 1, 1);
        
        StringKeyModel * stringModel = (StringKeyModel *) model;
        singleValue->setPlainText(stringModel->getValue());
    }
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

    gridLayout->addLayout(paginationGrid, 2, 0, 1, 2);

    connect(keyValue->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), 
        this, SLOT(onSelectedItemChanged(const QModelIndex &, const QModelIndex &)));

    paginatedModel = (PaginatedModel *) keyValue->model();

    int pagesCount = paginatedModel->getPagesCount();
    pagination->setText(
        QString("Page <b>1</b> of <b>%1</b> (Items: %2)")
            .arg(pagesCount)
            .arg(paginatedModel->itemsCount())
            );

    if (pagesCount > 1) {        
        nextPage->setEnabled(true);

        connect(nextPage, SIGNAL(clicked()), this, SLOT(loadNextPage()));
        connect(previousPage, SIGNAL(clicked()), this, SLOT(loadPreviousPage()));
    }
}

void ValueTabView::loadNextPage()
{
    int currentPage = paginatedModel->getCurrentPage();
    int totalPages = paginatedModel->getPagesCount();

    if (currentPage == totalPages) {
        return;
    }

    paginatedModel->setCurrentPage(++currentPage);

    if (currentPage == totalPages) {
        nextPage->setEnabled(false);
    }

    if (currentPage == 2) {
        previousPage->setEnabled(true);
    }

    pagination->setText(
        QString("Page <b>%1</b> of <b>%2</b> (Items: %3)")
            .arg(currentPage)
            .arg(totalPages)
            .arg(paginatedModel->itemsCount())
            );
}

void ValueTabView::loadPreviousPage()
{
    int currentPage = paginatedModel->getCurrentPage();
    int totalPages = paginatedModel->getPagesCount();

    if (currentPage == 1) {
        return;
    }

    paginatedModel->setCurrentPage(--currentPage);

    if (currentPage == totalPages - 1) {
        nextPage->setEnabled(true);
    }

    if (currentPage == 1) {
        previousPage->setEnabled(false);
    }

    pagination->setText(
        QString("Page <b>%1</b> of <b>%2</b> (Items: %3)")
            .arg(currentPage)
            .arg(totalPages)
            .arg(paginatedModel->itemsCount())
            );
}

void ValueTabView::onSelectedItemChanged(const QModelIndex & current, const QModelIndex & previous)
{
    Q_UNUSED(previous);

    singleValue->clear();    

    formatter->setSource(model->itemFromIndex(current)->text());

    singleValue->appendPlainText(formatter->getFormatted());

    currentCell = &current;
}

void ValueTabView::currentFormatterChanged(int index)
{
    AbstractFormatter::FormatterType newFormatterType = (AbstractFormatter::FormatterType)index;

    delete formatter;

    formatter = AbstractFormatter::getFormatter(newFormatterType);

    if (model->getKeyModelType() == StringKeyModel::KEY_MODEL_TYPE) {        
        singleValue->clear();
        formatter->setSource(((StringKeyModel*)model)->getValue());        
        singleValue->appendPlainText(formatter->getFormatted());
    } else if (currentCell != nullptr) {
        onSelectedItemChanged(*currentCell, *currentCell);
    }    
}

void ValueTabView::valueUpdate()
{
    if (currentCell == nullptr && model->getKeyModelType() != StringKeyModel::KEY_MODEL_TYPE)
        return;

    QString value = singleValue->toPlainText();

    formatter->setSource(value);

    if (!formatter->isValid()) {
        QMessageBox::warning(controller, "Invalid value", "Check value format");
        return;
    }

    emit saveChangedValue(formatter->getRaw(), currentCell);
}

const QModelIndex * ValueTabView::getCurrentCell()
{
    return currentCell;
}

void ValueTabView::showLoader()
{
    loaderLabel->show();
    loader->start();
}

void ValueTabView::hideLoader()
{
    loader->stop();
    loaderLabel->hide();
}
