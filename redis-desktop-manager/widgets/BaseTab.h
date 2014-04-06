#pragma once

#include <QWidget>

class BaseTab : public QWidget
{
    Q_OBJECT
public:
    virtual void close();

    virtual bool shouldBeReplaced();
};

