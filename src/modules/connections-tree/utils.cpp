#include "utils.h"
#include <QMessageBox>
#include <QKeySequence>
#include <qtranslator.h>

void ConnectionsTree::confirmAction(QWidget *parent, const QString &msg,
                                    std::function<void ()> action, QString title)
{
    QMessageBox::StandardButton reply = QMessageBox::question(parent, title,
                                                              msg, QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        action();
    }


}

void ConnectionsTree::confirmAction(QWidget *parent, const QString &msg, std::function<void ()> action,
                                    std::function<void ()> noCallback, QString title)
{
    QMessageBox::StandardButton reply = QMessageBox::question(parent, title,
                                                              msg, QMessageBox::No|QMessageBox::Yes,
                                                              QMessageBox::StandardButton::Yes);
    if (reply == QMessageBox::Yes) {
        action();
    } else {
        noCallback();
    }
}
