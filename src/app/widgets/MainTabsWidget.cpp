#include "maintabswidget.h"
#include <QTabBar>
#include <QMessageBox>

MainTabsWidget::MainTabsWidget(QWidget * parent)
    : QTabWidget(parent)
{
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(OnTabClose(int)));
}

int MainTabsWidget::addTab(QWidget*w, const QIcon&i, const QString &label)
{
    return QTabWidget::addTab(w, i, label);
}

int MainTabsWidget::addTab(QWidget*w, const QString &label)
{
    return QTabWidget::addTab(w, label);
}

int MainTabsWidget::addTab(QString& tabName, QWidget* tab, QString icon, bool forceOpenInNewTab)
{        
    int currIndex;

    if (!forceOpenInNewTab) {
        closeCurrentTabWithValue();
    }

    if (icon.isEmpty()) {
        currIndex = QTabWidget::addTab(tab, tabName);
    } else {
        currIndex = QTabWidget::addTab(tab, QIcon(icon), tabName);
    }

    setCurrentIndex(currIndex);

    return currIndex;
}

//void MainTabsWidget::openKeyTab(RedisKeyItem * key, bool inNewTab)
//{
//    QWidget * viewTab = new ValueTab(key);

//    connect(viewTab, SIGNAL(keyDeleted(QWidget *, RedisKeyItem *)),
//        this, SLOT(OnKeyDeleted(QWidget *, RedisKeyItem *)));

//    connect(viewTab, SIGNAL(error(const QString &)),
//        this, SLOT(OnError(const QString &)));

//    QString keyFullName = key->getTabLabelText();

//    if (inNewTab) {
//        addTab(keyFullName, viewTab, QString(), true);
//    } else {
//        addTab(keyFullName, viewTab);
//    }
//}

//void MainTabsWidget::OnKeyDeleted(QWidget * tab, RedisKeyItem * key)
//{
//    if (tab == nullptr || key == nullptr)
//        return;

//    int widgetsCount = count();
//    int widgetIndex = -1;

//    for (int currentWidget = 0; currentWidget < widgetsCount; currentWidget++)
//    {
//        if (tab == widget(currentWidget)) {
//            widgetIndex = currentWidget;
//            break;
//        }
//    }

//    if (widgetIndex == -1) {
//        return;
//    }

//    closeTab(widgetIndex);
//}

int MainTabsWidget::getTabIndex(QString& name)
{
    for (int i = 0; i < count(); ++i)
    {
        if (name == tabText(i)) {
            return i;                            
        }
    }

    return -1;
}

void MainTabsWidget::closeCurrentTabWithValue()
{
    int currIndex = currentIndex();

    if (currIndex == -1) 
        return;

    BaseTab * tab = qobject_cast<BaseTab *> (widget(currIndex));

    if (tab->shouldBeReplaced()) {
        closeTab(currIndex);
    }
}

void MainTabsWidget::closeTab(unsigned int index)
{
    BaseTab * tab = qobject_cast<BaseTab *> (widget(index));
    removeTab(index);
    tab->close();
}

void MainTabsWidget::addTab(QSharedPointer<BaseTab> tab)
{
    m_tabs.push_back(tab);
    QTabWidget::addTab(tab.data(), tab->getIcon(), tab->getTitle());
}

void MainTabsWidget::OnTabClose(int index)
{
    closeTab((unsigned int)index);
}

void MainTabsWidget::OnError(const QString &error)
{
    QMessageBox::warning(this, "Error", error);
}
