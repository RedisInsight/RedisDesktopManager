#include "valuetab.h"
#include <QQuickWidget>
#include <QUrl>
#include <QQmlEngine>
#include <QBoxLayout>

using namespace ValueEditor;

ValueTab::ValueTab(const QString& keyFullPath, QSharedPointer<ConnectionsTree::Operations> operations) :
    BaseTab(),
    m_keyFullPath(keyFullPath),
    m_operations(operations)

{
    QQuickWidget *quickWidget = new QQuickWidget(this);
    quickWidget->setSource(QUrl::fromLocalFile("qml/value-editor.qml"));

    QBoxLayout * layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    layout->setMargin(0);
    layout->addWidget(quickWidget);
    setLayout(layout);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
}


QString ValueTab::getTitle()
{
    return m_keyFullPath;
}
