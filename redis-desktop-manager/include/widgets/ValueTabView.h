#pragma once

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPlainTextEdit>
#include <QMovie>
#include <QPushButton>
#include <QSplitter>
#include <QComboBox>
#include <QGroupBox>

class KeyModel;

class ValueTabView
{
public:

	ValueTabView(const QString& name, QWidget * parent);

	QGridLayout *gridLayout;
	QLineEdit *keyName;
	QTableView *keyValue;
	QPlainTextEdit *singleValue;
	QGroupBox *singleValueGroup;
	QComboBox * singleValueFormatterType;
	QSplitter *splitter;
	QPlainTextEdit *keyValuePlain;
	QLabel *keyNameLabel;	
	QLabel *keyValueLabel;
	QLabel *keyTypeLabelValue;
	QLabel *loaderLabel;
	QMovie *loader;
	QLabel * formatterLabel;

	QPushButton * renameKey;
	QPushButton * deleteKey;

	QGridLayout * paginationGrid;
	QPushButton * previousPage;
	QPushButton * nextPage;
	QLabel * pagination;

	void initKeyValue(KeyModel *);

protected:
	QWidget * controller;

	void initLayout();

	void initKeyName();	

	void initPagination();

	void initFormatter();
};

