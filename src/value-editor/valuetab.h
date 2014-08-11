#ifndef VALUETAB_H
#define VALUETAB_H

#include "widgets/BaseTab.h"
#include "connections-tree/operations.h"

namespace ValueEditor {

class ValueTab : public BaseTab
{
    Q_OBJECT
public:
    ValueTab(const QString& keyFullPath, QSharedPointer<ConnectionsTree::Operations> operations);

    QString getTitle() override;

private:
    QString m_keyFullPath;
    QSharedPointer<ConnectionsTree::Operations> m_operations;
};
}

#endif // VALUETAB_H
