#include "tab.h"
#include <QQuickWidget>
#include <QUrl>
#include <QQmlEngine>
#include <QBoxLayout>
#include <QQmlContext>
#include <QDateTime>
#include "sharedqmlengine.h"

using namespace ValueEditor;

Tab::Tab() :
    BaseTab()
{

    static bool flag;

    //todo: use global defined QmlEngine	
    QQuickWidget *quickWidget = new QQuickWidget(SharedQmlEngine::getSharedEngine().data(), this);
    if (!flag) {
        quickWidget->setSource(QUrl(QStringLiteral("qrc:///qml/value-editor.qml")));
        flag = true;
    }
    quickWidget->rootContext()->setContextProperty("test", QDateTime::currentMSecsSinceEpoch());
    quickWidget->setFocus();

    QBoxLayout * layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    layout->setMargin(0);
    layout->addWidget(quickWidget);
    setLayout(layout);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
}
