#pragma once

#include <QtQuickTest>
#include <QQmlEngine>
#include <QQmlContext>
#include <QSharedPointer>
#include <QQuickTextDocument>
#include "app/qmlutils.h"

class TestUtils : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE void removeAppSetting(const QString& category);
};

class Setup : public QObject
{
    Q_OBJECT
public:
    Setup() {}

public slots:
    void qmlEngineAvailable(QQmlEngine *engine);

private:
    QSharedPointer<QmlUtils> m_qmlUtils;
    QSharedPointer<TestUtils> m_testUtils;
};
