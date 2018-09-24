#pragma once
#include <QString>
#include <QFile>
#include <QDir>
#include <QJsonArray>

class ConfigManager
{
public:
    ConfigManager(const QString& basePath = QDir::homePath());
    QString getApplicationConfigPath(const QString &, bool checkPath=true);    
public:
    static QString getConfigPath(QString basePath = QDir::homePath());    

private:
    static bool chechPath(const QString&);
    static void setPermissions(QFile&);    
private:
    QString m_basePath;
};

bool saveJsonArrayToFile(const QJsonArray& c, const QString& f);
