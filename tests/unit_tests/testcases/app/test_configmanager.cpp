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

void TestConfigManager::testMigrateOldConfig()
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

    // When
    // Case 1 - Migrate from old versions (<= 0.7.6)
    // cp connections.xml to temp dir
    QString targetConfigPath = QString("%1/connections.xml").arg(basePath);
    QVERIFY(QFile::copy("./unit_tests/testcases/app/connections.xml", targetConfigPath));

    bool actual_result = manager.migrateOldConfig("connections.xml", "connections.json");

    // Then
    QCOMPARE(true, actual_result);
    // check that connections.xml doesn't exist in base dir
    QCOMPARE(false, QFile::exists(targetConfigPath));
    // check that .rdm dir exist in base path
    QDir configDir(QString("%1/.rdm").arg(basePath));
    QCOMPARE(true, configDir.exists());
    // check that .rdm/connections.json exists
    QFile newConfig(QString("%1/.rdm/connections.json").arg(basePath));
    QCOMPARE(true, newConfig.exists());
    // check that .rdm/connections.xml.backup exists
    QFile oldConfigBackup(QString("%1/.rdm/connections.xml.backup").arg(basePath));
    QCOMPARE(true, oldConfigBackup.exists());
    // check that .rdm/connections.xml doesn't exist
    QCOMPARE(false, QFile::exists(QString("%1/.rdm/connections.xml").arg(basePath)));
    // check that backup and new config is not empty
    QCOMPARE(true, newConfig.size() > 0);
    QCOMPARE(true, oldConfigBackup.size() > 0);

    // Clean run - Run on (>= 0.8.0) when connections.xml doesn't exist
    // Check that connections.xml not created
    actual_result = manager.migrateOldConfig("connections.xml", "connections.json");
    QCOMPARE(false, QFile::exists(targetConfigPath));
    QCOMPARE(false, QFile::exists(QString("%1/.rdm/connections.xml").arg(basePath)));

    // Negative run
    // connections.xml, .rdm/connections.xml and .rdm/connections.json exist
    QVERIFY(QFile::copy("./unit_tests/testcases/app/connections.xml", targetConfigPath));
    QVERIFY(QFile::copy("./unit_tests/testcases/app/connections.xml",
                        QString("%1/.rdm/connections.xml").arg(basePath)));
    QFileInfo configInfo(newConfig);
    QDateTime currentModificationTime = configInfo.lastModified();

    actual_result = manager.migrateOldConfig("connections.xml", "connections.json");
    // Check that old files are untouched and connections.json is not updated
    QCOMPARE(false, actual_result);
    QCOMPARE(true, QFile::exists(targetConfigPath));
    QCOMPARE(true, QFile::exists(QString("%1/.rdm/connections.xml").arg(basePath)));
    configInfo.refresh();
    QCOMPARE(currentModificationTime, configInfo.lastModified());
}

void TestConfigManager::testXmlConfigToJsonArray()
{
    // Given
    QString pathToXmlConfig("./unit_tests/testcases/app/connections.xml");

    // When
    QJsonArray actual_result = ConfigManager::xmlConfigToJsonArray(pathToXmlConfig);

    // Then
    // Check that resulting array contains namespace separator if it missing in original XML
    for (auto connection : actual_result) {
        QCOMPARE(QString(":"), connection.toObject()["namespace_separator"].toString());
    }
}
