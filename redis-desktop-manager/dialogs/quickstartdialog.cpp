#include "quickstartdialog.h"
#include "ui_quickstartdialog.h"

QuickStartDialog::QuickStartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuickStartDialog)
{
    ui->setupUi(this);
}

QuickStartDialog::~QuickStartDialog()
{
    delete ui;
}
