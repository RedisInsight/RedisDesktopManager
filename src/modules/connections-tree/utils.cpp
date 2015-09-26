#include "utils.h"
#include <QMessageBox>
#include <qtranslator.h>

QAction *ConnectionsTree::createMenuAction(const QString &iconRef, const QString &title, QMenu *parent, QObject *owner, std::function<void ()> callback)
{
    QAction* action = new QAction(QIcon(iconRef), title, parent);
    QObject::connect(action, &QAction::triggered, owner, callback);
    return action;
}


void ConnectionsTree::confirmAction(QWidget *parent, const QString &msg,
                                    std::function<void ()> action, QString title)
{
    QMessageBox::StandardButton reply = QMessageBox::question(parent, title,
                                                              msg, QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        action();
    }
}
