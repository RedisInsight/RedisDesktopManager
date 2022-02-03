#include "setup.h"

#include <QSettings>

#include "modules/value-editor/syntaxhighlighter.h"
#include "modules/value-editor/textcharformat.h"

void Setup::qmlEngineAvailable(QQmlEngine *engine)
{
    QCoreApplication::instance()->setOrganizationDomain("redisdesktop.com");
    QCoreApplication::instance()->setOrganizationName("redisdesktop");
    QCoreApplication::instance()->setApplicationName("RESP.app - Developer GUI for Redis");

    qmlRegisterType<SyntaxHighlighter>("rdm.models", 1, 0, "SyntaxHighlighter");
    qmlRegisterType<TextCharFormat>("rdm.models", 1, 0, "TextCharFormat");

    m_qmlUtils = QSharedPointer<QmlUtils>(new QmlUtils());
    engine->rootContext()->setContextProperty("qmlUtils", m_qmlUtils.data());

    m_testUtils = QSharedPointer<TestUtils>(new TestUtils());
    engine->rootContext()->setContextProperty("testUtils", m_testUtils.data());
}

void TestUtils::removeAppSetting(const QString &category)
{
    QSettings s;
    s.remove(category);
}
