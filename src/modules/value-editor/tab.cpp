#include "tab.h"
#include <QQuickWidget>
#include <QUrl>
#include <QQmlEngine>
#include <QBoxLayout>

using namespace ValueEditor;

Tab::Tab() :
    BaseTab()
{

    //todo: use global defined QmlEngine	
    QQuickWidget *quickWidget = new QQuickWidget(this);
    quickWidget->setSource(QUrl::fromLocalFile("qml/value-editor.qml"));

    QBoxLayout * layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    layout->setMargin(0);
    layout->addWidget(quickWidget);
    setLayout(layout);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
}
