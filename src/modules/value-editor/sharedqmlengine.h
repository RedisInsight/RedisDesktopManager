#pragma once
#include <QQmlEngine>
#include <QSharedPointer>

namespace ValueEditor {

class SharedQmlEngine
{
public:
    static QSharedPointer<QQmlEngine> getSharedEngine();

private:
    static QSharedPointer<QQmlEngine> m_engine;
};

}
