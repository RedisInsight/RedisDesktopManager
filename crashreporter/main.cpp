/* 
 * RedisDesktopManager Crash Reporter
 * based on source from http://tomahawk-player.org
 */
#include <iostream>
#include "CrashReporter.h"
#include <QApplication>
#include <QMessageBox>
#include <QUrl>
#include <QDebug>

int main( int argc, char* argv[] )
{
    QApplication app( argc, argv );

    if ( argc != 2 )
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("RedisDesktopManager Crash Reporter");
        msgBox.setText("<b>Usage:</b> <br /> crashReporter <i>dumpFileName</i>\n");
        msgBox.exec();
        return 1;
    }

    QApplication::setApplicationName( "RedisDesktopManager Crash Reporter");
    QApplication::setApplicationVersion( "1.0.0" );    
    QApplication::setOrganizationDomain( "redisdesktop.com" );    

    CrashReporter reporter( QUrl( "http://redisdesktop.com/crash-report" ),  app.arguments() );
    reporter.show();

    return app.exec();
}
