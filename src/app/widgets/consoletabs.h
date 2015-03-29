#pragma once

#include <QTabWidget>
#include "basetab.h"

class ConsoleTabs : public QTabWidget
{
    Q_OBJECT

public:
    ConsoleTabs(QWidget * parent = nullptr);

    // App interface, for functional tabs
    void addTab(QSharedPointer<BaseTab> tab);
    void closeTab(unsigned int index);
    void closeAllTabsWithName(const QString& name);

protected:
    void closeCurrentTabWithValue();

    /** @return >=0 if exist **/
    int getTabIndex(QString&);    

protected slots:
    void OnError(const QString &);
    void OnTabClose(int i);    

private:
    QList<QSharedPointer<BaseTab>> m_tabs;
};

