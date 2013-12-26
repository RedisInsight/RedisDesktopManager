#pragma once

#include <QTabWidget>

class RedisServerItem;
class RedisKeyItem;

class MainTabsWidget : public QTabWidget
{
    Q_OBJECT

public:
    MainTabsWidget(QWidget * parent = nullptr);

    int addTab(QString&, QWidget*, QString icon = QString(), bool forceOpenInNewTab = false);    

    int addTab(QWidget*, const QIcon&, const QString &);

    int addTab(QWidget*, const QString &);

    void closeTab(unsigned int index);
    
    void openKeyTab(RedisKeyItem * key, bool inNewTab = false);    

protected:
    void closeCurrentTabWithValue();

    /** @return >=0 if exist **/
    int getTabIndex(QString&);    

protected slots:
    void OnTabClose(int i);
    void OnKeyDeleted(QWidget * tab, RedisKeyItem * key);
};

