#include "configmanager.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>
#include <QVariantHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <easylogging++.h>
#include <qredisclient/utils/compat.h>
#include "app/models/connectionconf.h"

ConfigManager::ConfigManager(const QString &basePath)
    : m_basePath(basePath)
{
}

QString ConfigManager::getApplicationConfigPath(const QString &configFile, bool checkPath)
{
    QString configDir = getConfigPath(m_basePath);
    QDir settingsPath(configDir);

    if (!settingsPath.exists() && settingsPath.mkpath(configDir)) {
        qDebug() << "Config Dir created";
    }

    QString configPath = QString("%1/%2").arg(configDir).arg(configFile);

    if (checkPath && !chechPath(configPath))
        return QString();

    return configPath;
}

/**
 * @brief ConfigManager::migrateOldConfig
 * @param oldFileName - config.xml
 * @param newFileName - config.json
 * @return true or false
 */
bool ConfigManager::migrateOldConfig(const QString &oldFileName, const QString &newFileName)
{
    QString jsonConfigPath = getApplicationConfigPath(newFileName, false);

    if (QFile::exists(jsonConfigPath)) {
        qDebug() << "New config already exist. Ignore old configs.";
        return false;
    }

    // Move config from 0.7.5 or older to appropriate directory
    QString homeConfig = QString("%1/%2").arg(m_basePath).arg(oldFileName);

    QString xmlConfigPath = getApplicationConfigPath(oldFileName, false);

    if (xmlConfigPath.isEmpty())
        return false;

    if (QFile::exists(homeConfig)) {
        qDebug() << "Config migration: 0.7.5 config detected.";
        QFile::remove(xmlConfigPath);
        if (QFile::copy(homeConfig, xmlConfigPath)
                && QFile::remove(homeConfig)) {
            qDebug() << "Old config moved to new dir";
        }
    }

    if (!QFile::exists(xmlConfigPath))
        return false;

    QJsonArray newConfig = xmlConfigToJsonArray(xmlConfigPath);

    QFile::rename(xmlConfigPath, QString("%1.backup").arg(xmlConfigPath));

    if (newConfig.size() == 0)
        return false;       

    return saveJsonArrayToFile(newConfig, jsonConfigPath);
}

/**
 * @brief ConfigManager::xmlConfigToJsonArray - Migrate XML config to JSON array
 * @param xmlConfigPath
 * @return
 */
QJsonArray ConfigManager::xmlConfigToJsonArray(const QString &xmlConfigPath)
{
    LOG(WARNING) << "XML support is deprecated and will be removed in RDM 1.0";
    QJsonArray newConfig;

    QFile* xmlConfigfile = new QFile(xmlConfigPath);
    if (!xmlConfigfile->open(QIODevice::ReadOnly | QIODevice::Text))
        return newConfig;

    QXmlStreamReader xml(xmlConfigfile);

    QHash<QString, QString> attrMapping ({
        {"sshHost", "ssh_host"},
        {"sshUser", "ssh_user"},
        {"sshPassword", "ssh_password"},
        {"sshPort", "ssh_port"},
        {"sshPrivateKey", "ssh_private_key_path"},
        {"namespaceSeparator", "namespace_separator"},
        {"connectionTimeout", "timeout_connect"},
        {"executeTimeout", "timeout_execute"},
    });

    while (!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartDocument)
            continue;
        if (token == QXmlStreamReader::StartElement)
        {
            if (xml.name() == "connections")
                continue;
            if (xml.name() == "connection") {
                QXmlStreamAttributes attributes = xml.attributes();

                QString name, value;
                QVariantHash connection;

                for (QXmlStreamAttribute attr : attributes) {
                    name = attr.name().toString();
                    if (attrMapping.contains(name)) {
                        name = attrMapping[name];
                    }

                    value = attr.value().toString();

                    if (name.contains("port") || name.contains("timeout")) {
                        connection.insert(name, value.toInt());
                    } else {
                        connection.insert(name, value);
                    }
                }

                // NOTE(u_glide): We should add NS separator to new config
                if (!connection.contains("namespace_separator")) {
                    connection.insert("namespace_separator",
                                      QString(ServerConfig::DEFAULT_NAMESPACE_SEPARATOR));
                }

                newConfig.append(QJsonObjectFromVariantHash(connection));
            }
        }
    }       
    return newConfig;
}

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

QString ConfigManager::getConfigPath(QString basePath)
{
    QString configDir;
#ifdef Q_OS_MACX
    if (basePath == QDir::homePath()) {
        configDir = "/Library/Preferences/rdm/";
    } else {
        configDir = ".rdm";
    }

    configDir = QDir::toNativeSeparators(
                    QString("%1/%2").arg(basePath).arg(configDir)
                );
#else
    configDir = QDir::toNativeSeparators(
                    QString("%1/%2").arg(basePath).arg(".rdm")
                );
#endif
    return configDir;
}

bool saveJsonArrayToFile(const QJsonArray &c, const QString &f)
{
    QJsonDocument config(c);
    QFile confFile(f);

    if (confFile.open(QIODevice::WriteOnly)) {
        QTextStream outStream(&confFile);
        outStream.setCodec("UTF-8");
        outStream << config.toJson();
        confFile.close();
        return true;
    }
    return false;
}
