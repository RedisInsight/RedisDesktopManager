#pragma once
#include <QWidget>
#include <QList>
#include <QSharedPointer>
#include <QVariantList>
#include <QQuickView>
#include <QQuickWidget>
#include "viewmodel.h"
#include "binary.h"

namespace BulkOperations {
    class Manager;
}

namespace ValueEditor {

class View : public QWidget
{
    Q_OBJECT    
public:
    View(QSharedPointer<ViewModel> viewModel, QSharedPointer<BulkOperations::Manager> bulkOperations);
private:
    QSharedPointer<QQuickWidget> m_qml;
    QSharedPointer<BinaryUtils> m_binaryUtils;
};
}
