#pragma once

#include <QWidget>
#include <QIcon>

class BaseTab : public QWidget
{
    Q_OBJECT
public:
    virtual void close() {}
    virtual QString getTitle() {return "Tab";}
    virtual QIcon getIcon() {return QIcon();}
};

