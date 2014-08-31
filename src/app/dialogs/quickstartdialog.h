#ifndef QUICKSTARTDIALOG_H
#define QUICKSTARTDIALOG_H

#include <QDialog>

namespace Ui {
class QuickStartDialog;
}

class QuickStartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QuickStartDialog(QWidget *parent = 0);
    ~QuickStartDialog();

private:
    Ui::QuickStartDialog *ui;
};

#endif // QUICKSTARTDIALOG_H
