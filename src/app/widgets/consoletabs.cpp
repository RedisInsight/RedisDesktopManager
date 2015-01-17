#include "consoletabs.h"
#include <QTabBar>
#include <QMessageBox>

ConsoleTabs::ConsoleTabs(QWidget * parent)
    : QTabWidget(parent)
{
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(OnTabClose(int)));
}

int ConsoleTabs::getTabIndex(QString& name)
{
    for (int i = 0; i < count(); ++i)
    {
        if (name == tabText(i)) {
            return i;                            
        }
    }

    return -1;
}

void ConsoleTabs::closeCurrentTabWithValue()
{
    int currIndex = currentIndex();

    if (currIndex == -1) 
        return;

    BaseTab * tab = qobject_cast<BaseTab *> (widget(currIndex));

    //if (tab->shouldBeReplaced()) {
        closeTab(currIndex);
    //}
}

void ConsoleTabs::closeTab(unsigned int index)
{
    BaseTab * tab = qobject_cast<BaseTab *> (widget(index));
    removeTab(index);
    tab->close();
}

void ConsoleTabs::closeAllTabsWithName(const QString &name)
{
    int widgetsCount = count();

    for (int currentIndex = 0; currentIndex < widgetsCount; currentIndex++)
    {
        if (tabText(currentIndex) == name) {
            this->closeTab(currentIndex);
        }
    }
}

void ConsoleTabs::addTab(QSharedPointer<BaseTab> tab)
{
    m_tabs.push_back(tab);
    int index = QTabWidget::addTab(tab.data(), tab->getIcon(), tab->getTitle());
    setCurrentIndex(index);
    tab->setFocus();
}

void ConsoleTabs::replaceCurrentTab(QSharedPointer<BaseTab> tab)
{

}

void ConsoleTabs::OnTabClose(int index)
{
    closeTab((unsigned int)index);
}

void ConsoleTabs::OnError(const QString &error)
{
    QMessageBox::warning(this, "Error", error);
}
