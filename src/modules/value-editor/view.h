#pragma once

#include <QWidget>
#include <QList>
#include <QSharedPointer>
#include <QVariantList>
#include <QQuickView>
#include <QQuickWidget>
#include "viewmodel.h"

#define ALTERNATIVE_IMPL

namespace ValueEditor {

class View : public QWidget
{
    Q_OBJECT    
public:
    View(QSharedPointer<ViewModel> viewModel);
private:
    QSharedPointer<QQuickWidget> m_qml;
};
}
