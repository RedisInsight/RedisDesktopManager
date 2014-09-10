#pragma once

#include <QTabWidget>
#include "basetab.h"

class MainTabsWidget : public QTabWidget
{
    Q_OBJECT

public:
    MainTabsWidget(QWidget * parent = nullptr);

    int addTab(QString&, QWidget*, QString icon = QString(), bool forceOpenInNewTab = false);    

    int addTab(QWidget*, const QIcon&, const QString &);

    int addTab(QWidget*, const QString &);

    void closeTab(unsigned int index);

    void addTab(QSharedPointer<BaseTab> tab);
    
//    void openKeyTab(RedisKeyItem * key, bool inNewTab = false);

protected:
    void closeCurrentTabWithValue();

    /** @return >=0 if exist **/
    int getTabIndex(QString&);    

protected slots:
    void OnError(const QString &);
    void OnTabClose(int i);
    //void OnKeyDeleted(QWidget * tab, RedisKeyItem * key);

private:
    QList<QSharedPointer<BaseTab>> m_tabs;
};

