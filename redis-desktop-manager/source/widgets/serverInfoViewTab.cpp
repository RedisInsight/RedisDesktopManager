#include "serverInfoViewTab.h"

#include <QStringListModel>

serverInfoViewTab::serverInfoViewTab(QString serverName, QStringList info)
{
	ui.setupUi(this);

	ui.serverName->setText(serverName);

	QStringListModel *model = new QStringListModel(info);

	ui.serverInfoView->setModel(model);
}
