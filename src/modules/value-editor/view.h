#ifndef VALUETAB_H
#define VALUETAB_H

#include <QWidget>
#include <QList>
#include <QSharedPointer>
#include <QVariantList>
#include <QQuickView>
#include "viewmodel.h"

namespace ValueEditor {

class View : public QWidget
{
    Q_OBJECT    
public:
    View(QSharedPointer<ViewModel> viewModel);

private:
    QSharedPointer<QQuickView> m_qml;
};
}

#endif // VALUETAB_H
