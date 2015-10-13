#include "quickstartdialog.h"
#include "ui_quickstartdialog.h"
#include <googlemp.h>

QuickStartDialog::QuickStartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuickStartDialog)
{
    ui->setupUi(this);

    GoogleMP::instance()->showScreen("quick-start-dialog");
}

QuickStartDialog::~QuickStartDialog()
{
    delete ui;
}
