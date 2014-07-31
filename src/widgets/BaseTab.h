#pragma once

#include <QWidget>

class BaseTab : public QWidget
{
    Q_OBJECT
public:
    virtual void close() = 0;
    virtual bool shouldBeReplaced() = 0;
};

