#include "sharedqmlengine.h"

using namespace ValueEditor;

QSharedPointer<QQmlEngine> ValueEditor::SharedQmlEngine::m_engine = QSharedPointer<QQmlEngine>();

QSharedPointer<QQmlEngine> SharedQmlEngine::getSharedEngine()
{
    if (m_engine.isNull())
        m_engine = QSharedPointer<QQmlEngine>(new QQmlEngine());

    return m_engine;
}
