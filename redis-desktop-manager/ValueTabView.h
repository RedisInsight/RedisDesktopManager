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

class ValueTabView
{
public:
	QGridLayout *gridLayout;
	QLineEdit *keyName;
	QTableView *keyValue;
	QPlainTextEdit *keyValuePlain;
	QLabel *keyNameLabel;	
	QLabel *keyValueLabel;

	enum Type { ModelBased, PlainBased };

	void init(QWidget * baseController, ValueTabView::Type t = ModelBased);

	void setModel(QAbstractItemModel * model);

	void setPlainValue(QString &);

	ValueTabView::Type getType();

protected:
	QWidget * controller;
	ValueTabView::Type viewType;

	void initLayout();

	void initKeyName();

	void initKeyValue();
};

