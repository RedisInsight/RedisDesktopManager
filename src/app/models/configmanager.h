#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>
#include <QFile>
#include <QJsonArray>

class ConfigManager
{
public:
    static QString getApplicationConfigPath(const QString &);
    static bool migrateOldConfig(const QString &oldFileName, const QString &newFileName);
    static QJsonArray xmlConfigToJsonArray(const QString &xmlConfigPath);

private:
    static bool chechPath(const QString&);
    static void setPermissions(QFile&);
};

#endif // CONFIGMANAGER_H
