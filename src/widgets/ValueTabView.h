#pragma once

#include <QtCore>
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
//#include "RedisKeyItem.h"

class KeyModel;
class PaginatedModel;
class AbstractFormatter;

class ValueTabView : public QObject
{
    Q_OBJECT

public:
    //ValueTabView(RedisKeyItem * key, const QString& name, QWidget * parent);

    QLineEdit *keyName;
    QLabel *keyTypeLabelValue;
    QPushButton * renameKey;
    QPushButton * deleteKey;

    void initKeyValue(KeyModel *);

    void setKeyExpire(int);

    const QModelIndex * getCurrentCell();

    void showLoader();
    void hideLoader();

protected:
    QWidget * controller;
    KeyModel * model;
    PaginatedModel * paginatedModel;
    AbstractFormatter * formatter;
    const QModelIndex * currentCell;

    // UI items
    QTableView *keyValue;
    QPlainTextEdit *singleValue;
    QGroupBox *singleValueGroup;
    QComboBox * singleValueFormatterType;
    QSplitter *splitter;
    QLabel *keyNameLabel;    
    QLabel *keyValueLabel;
    QLabel * keyExpireLabel;
    QLabel *loaderLabel;
    QMovie *loader;
    QLabel * formatterLabel;
    QGridLayout * paginationGrid;
    QPushButton * previousPage;
    QPushButton * nextPage;
    QLabel * pagination;
    QGridLayout *gridLayout;
    QPushButton * saveValue;

    void initLayout();

    void initKeyName();    

    void initPagination();

    void initFormatter(const QString&);

protected slots:
    void loadNextPage();
    void loadPreviousPage();
    void onSelectedItemChanged(const QModelIndex & current, const QModelIndex & previous);
    void currentFormatterChanged(int index);
    void valueUpdate();

signals:
    void saveChangedValue(const QString& value, const QModelIndex * currCell);
};

