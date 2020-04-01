#include "configmanager.h"
#include <qredisclient/utils/compat.h>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantHash>
#include <QXmlStreamReader>
#include "app/models/connectionconf.h"

ConfigManager::ConfigManager(const QString &basePath) : m_basePath(basePath) {}

QString ConfigManager::getApplicationConfigPath(const QString &configFile,
                                                bool checkPath) {
  QString configDir = getConfigPath(m_basePath);
  QDir settingsPath(configDir);

  if (!settingsPath.exists() && settingsPath.mkpath(configDir)) {
    qDebug() << "Config Dir created";
  }

  QString configPath = QString("%1/%2").arg(configDir).arg(configFile);

  if (checkPath && !chechPath(configPath)) return QString();

  return configPath;
}

bool ConfigManager::chechPath(const QString &configPath) {
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

void ConfigManager::setPermissions(QFile &file) {
#ifdef Q_OS_WIN
  extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
  qt_ntfs_permission_lookup++;
#endif

  if (!file.setPermissions(QFile::ReadUser | QFile::WriteUser))
    qWarning() << "Cannot set permissions for config folder";

#ifdef Q_OS_WIN
  qt_ntfs_permission_lookup--;
#endif
}

QString ConfigManager::getConfigPath(QString basePath) {
  QString configDir;
#ifdef Q_OS_MACX
  if (basePath == QDir::homePath()) {
    configDir = "/Library/Preferences/rdm/";
  } else {
    configDir = ".rdm";
  }

  configDir =
      QDir::toNativeSeparators(QString("%1/%2").arg(basePath).arg(configDir));
#else
  configDir =
      QDir::toNativeSeparators(QString("%1/%2").arg(basePath).arg(".rdm"));
#endif
  return configDir;
}

bool saveJsonArrayToFile(const QJsonArray &c, const QString &f) {
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
