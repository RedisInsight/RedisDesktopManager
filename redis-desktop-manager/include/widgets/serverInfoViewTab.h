#pragma once

#include <QWidget>
#include "ui_serverInfoViewTab.h"

class serverInfoViewTab: public QWidget
{
    Q_OBJECT
public:
    serverInfoViewTab(QString, QStringList);

private:
    Ui::serverInfoViewTab ui;
};

