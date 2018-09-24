#include "test_configmanager.h"
#include "app/models/configmanager.h"

#include <QTemporaryDir>
#include <QFile>

void TestConfigManager::testGetApplicationConfigPath()
{
#ifdef Q_OS_MACX
    QSKIP("SKIP ON OSX");
#endif
#ifdef Q_OS_WIN
    QSKIP("SKIP ON Windows");
#endif
    // Given
    QTemporaryDir tmpDir;
    tmpDir.setAutoRemove(true);
    QString basePath = tmpDir.path();
    ConfigManager manager(basePath);
    qDebug() << "Base path:" << basePath;
    bool check_path = true;

    // When
    QString actual_result = manager.getApplicationConfigPath("config.json", check_path);

    // Then
    // Check that path is valid
    QCOMPARE(QString("%1/.rdm/config.json").arg(basePath),
             actual_result);
    QCOMPARE(check_path, QFile::exists(actual_result));
}
