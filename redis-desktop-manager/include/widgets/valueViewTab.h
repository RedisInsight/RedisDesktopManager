#ifndef HASHVIEWTAB_H
#define HASHVIEWTAB_H

#include <QWidget>
#include <QStandardItemModel>
#include "ui_hashViewTab.h"
#include "ui_listViewTab.h"
#include "ui_stringViewTab.h"
#include "ui_zsetViewTab.h"
#include "RedisKeyItem.h"

class valueViewTab : public QWidget
{
	Q_OBJECT

public:
	valueViewTab(RedisKeyItem * key);
	~valueViewTab();

protected:
	void init();
	RedisKeyItem * key;
	RedisKeyItem::Type type;
	QStandardItemModel * model;

	Ui::stringViewTab * uiString;
	Ui::hashViewTab * uiHash;
	Ui::listViewTab * uiList;
	Ui::zsetViewTab * uiZSet;

	template < class T >
	inline void initUI(T * ui, QString keyName)
	{
		ui->setupUi(this);	
		ui->keyName->setText(keyName);
	}

	template< class T >
	void initValueView(T * ui,  QStandardItemModel * model) {
		ui->keyValue->setModel(model);
		ui->keyValue->setVisible(false);
		ui->keyValue->resizeRowsToContents();
		ui->keyValue->setVisible(true);
	}

protected slots:
	void valueLoaded(const QVariant&, QObject *);
	//void error(QString&);
};



#endif // HASHVIEWTAB_H
