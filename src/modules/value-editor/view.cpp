#include "view.h"
#include <QUrl>
#include <QQmlEngine>
#include <QBoxLayout>
#include <QQmlContext>
#include <QDateTime>
#include <QApplication>
#include <QtQml>
#include <QSysInfo>
#include <QSortFilterProxyModel>
#include <googlemp.h>
#include "viewmodel.h"
#include "valueviewmodel.h"
#include "sortfilterproxymodel.h"

#include "modules/bulk-operations/bulkoperationsmanager.h"

using namespace ValueEditor;

static QObject *analytics_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    GoogleMP *gmp = GoogleMP::instance();
    return gmp;
}


View::View(QSharedPointer<ViewModel> viewModel, QSharedPointer<BulkOperations::Manager> bulkOperations)
    : QWidget(), m_qml(nullptr),
      m_binaryUtils(QSharedPointer<BinaryUtils>(new BinaryUtils()))
{    
    m_qml = QSharedPointer<QQuickWidget>(new QQuickWidget());

    qmlRegisterType<ValueViewModel>("rdm.models", 1, 0, "ValueViewModel");
    qmlRegisterType<SortFilterProxyModel>("rdm.models", 1, 0, "SortFilterProxyModel");
    qmlRegisterSingletonType<GoogleMP>("MeasurementProtocol", 1, 0, "Analytics", analytics_singletontype_provider);

    m_qml->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_qml->rootContext()->setContextProperty("appVersion", QApplication::applicationVersion());
    m_qml->rootContext()->setContextProperty("viewModel", viewModel.data());    
    m_qml->rootContext()->setContextProperty("rdm_platform", QSysInfo::productType());        
    m_qml->rootContext()->setContextProperty("binaryUtils", m_binaryUtils.data());
    m_qml->rootContext()->setContextProperty("bulkOperations", bulkOperations.data());

    m_qml->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    m_qml->setSource(QUrl(QStringLiteral("qrc:///qml/value-editor.qml")));

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    layout->setMargin(0);
    layout->addWidget(m_qml.data());

    setLayout(layout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumHeight(500);
}
