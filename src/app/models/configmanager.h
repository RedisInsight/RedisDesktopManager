#pragma once
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

bool saveJsonArrayToFile(const QJsonArray& c, const QString& f);
