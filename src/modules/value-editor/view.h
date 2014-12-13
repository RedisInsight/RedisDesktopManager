#ifndef VALUETAB_H
#define VALUETAB_H

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
#ifndef ALTERNATIVE_IMPL
    QSharedPointer<QQuickView> m_qml;
#else
    QSharedPointer<QQuickWidget> m_qml;
#endif
};
}

#endif // VALUETAB_H
