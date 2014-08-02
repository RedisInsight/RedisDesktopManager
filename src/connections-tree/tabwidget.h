#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QSharedPointer>

class BaseTab;

namespace ConnectionsTree {

class TabWidget
{
public:
    virtual void addTab(QSharedPointer<BaseTab> tab) = 0;
};

}
#endif // TABWIDGET_H
