#include "configmanager.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>

QString ConfigManager::getApplicationConfigPath(const QString &configFile)
{
#ifdef Q_OS_MACX
    QString libraryDir = QDir::toNativeSeparators(QString("%1/%2")
                                                  .arg(QDir::homePath())
                                                  .arg("/Library/Preferences/rdm/"));
#else
    QString libraryDir = QDir::toNativeSeparators(QString("%1/%2")
                                                  .arg(QDir::homePath())
                                                  .arg("/.rdm/"));
#endif
    QDir libraryPath(libraryDir);
    if (libraryPath.mkpath(libraryDir))
        qDebug() << "Config Dir created";

    QString configDir;

    if (libraryPath.exists()) {

        //config migration
        QString homeConfig = QString("%1/%2").arg(QDir::homePath()).arg(configFile);
        QString destConfig = QString("%1/%2").arg(libraryDir).arg(configFile);

        if (QFile::exists(homeConfig)
                && QFile::copy(homeConfig, destConfig)
                && QFile::remove(homeConfig)) {
            qDebug() << "Old config moved to new dir";
        }

        configDir = libraryDir;
    } else {
        configDir = QDir::homePath();
    }

    QString configPath = QString("%1/%2").arg(configDir).arg(configFile);

    if (!chechPath(configPath))
        return QString();

    return configPath;
}


/*
 * Convert XML config to JSON config:
 * Value mapping:
 *
 *     valueMapping.insert("sshHost", "ssh_host");
    valueMapping.insert("sshUser", "ssh_user");
    valueMapping.insert("sshPassword", "ssh_password");
    valueMapping.insert("sshPort", "ssh_port");
    valueMapping.insert("sshPrivateKey", "ssh_private_key_path");
    valueMapping.insert("namespaceSeparator", "namespace_separator");
    valueMapping.insert("connectionTimeout", "timeout_connect");
    valueMapping.insert("executeTimeout", "timeout_execute");
 *
 *
 */

bool ConfigManager::chechPath(const QString &configPath)
{
    QFile testConfig(configPath);
    QFileInfo checkPermissions(configPath);

    if (!testConfig.exists() && testConfig.open(QIODevice::ReadWrite))
        testConfig.close();

    if (checkPermissions.isWritable()) {
        setPermissions(testConfig);
        return true;
    }

    return false;
}

void ConfigManager::setPermissions(QFile &file)
{
#ifdef Q_OS_WIN
    extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
    qt_ntfs_permission_lookup++;
#endif

    if (file.setPermissions(QFile::ReadUser|QFile::WriteUser))
        qDebug() << "Permission changed";

#ifdef Q_OS_WIN
    qt_ntfs_permission_lookup--;
#endif
}
