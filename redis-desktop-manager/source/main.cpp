#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QSettings>

#include "CrashHandler.h"
#include "version.h"
#include "application.h"

int main(int argc, char *argv[])
{       
    QApplication a(argc, argv);

    CrashHandler::instance()->Init(QDir::homePath());

    QApplication::setApplicationName("Redis Desktop Manager");
    QApplication::setApplicationVersion(RDM_VERSION);    
    QApplication::setOrganizationDomain("redisdesktop.com");
    

#if defined(Q_OS_LINUX)
    QSettings settings;
    QVariant fontName = settings.value("font/name", QVariant("DejaVu Sans"));
    QVariant fontSize = settings.value("font/size", QVariant(9));

    QFont defaultFont(fontName.toString(), fontSize.toInt());
    defaultFont.setStyleHint(QFont::SansSerif);
    QApplication::setFont(defaultFont);

    settings.setValue("font/name", fontName);
    settings.setValue("font/size", fontSize);
#endif

    MainWin w;
    w.show();
    return a.exec();
}

