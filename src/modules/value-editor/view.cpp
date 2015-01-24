#include "view.h"
#include <QUrl>
#include <QQmlEngine>
#include <QBoxLayout>
#include <QQmlContext>
#include <QDateTime>
#include <QApplication>
#include <QtQml>
#include "viewmodel.h"
#include "valueviewmodel.h"

using namespace ValueEditor;


View::View(QSharedPointer<ViewModel> viewModel)
    : QWidget(), m_qml(nullptr)
{    
#ifndef ALTERNATIVE_IMPL

    m_qml = QSharedPointer<QQuickView>(new QQuickView());

    qmlRegisterType<ValueViewModel>("rdm.models", 1, 0, "ValueViewModel");

    m_qml->setResizeMode(QQuickView::SizeRootObjectToView);
    m_qml->rootContext()->setContextProperty("appVersion", QApplication::applicationVersion());
    m_qml->rootContext()->setContextProperty("viewModel", viewModel.data());
    m_qml->setSource(QUrl(QStringLiteral("qrc:///qml/value-editor.qml")));

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    layout->setMargin(0);    
    layout->addWidget(QWidget::createWindowContainer(m_qml.data()));
    setLayout(layout);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
#else
    m_qml = QSharedPointer<QQuickWidget>(new QQuickWidget());

    qmlRegisterType<ValueViewModel>("rdm.models", 1, 0, "ValueViewModel");

    m_qml->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_qml->rootContext()->setContextProperty("appVersion", QApplication::applicationVersion());
    m_qml->rootContext()->setContextProperty("viewModel", viewModel.data());    

    m_qml->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    m_qml->setSource(QUrl(QStringLiteral("qrc:///qml/value-editor.qml")));

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    layout->setMargin(0);
    layout->addWidget(m_qml.data());

    setLayout(layout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumHeight(500);
#endif

}
