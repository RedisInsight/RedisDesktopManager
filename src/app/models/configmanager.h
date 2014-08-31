#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>
#include <QFile>

class ConfigManager
{
public:
    static QString getApplicationConfigPath(const QString &);

private:
    static bool chechPath(const QString&);
    static void setPermissions(QFile&);
};

#endif // CONFIGMANAGER_H
