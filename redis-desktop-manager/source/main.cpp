#include "CrashHandler.h"
#include "demo.h"
#include <QApplication>

int main(int argc, char *argv[])
{	
	CrashHandler::instance()->Init(QDir::homePath());

	QApplication a(argc, argv);

	QApplication::setApplicationName("Redis Desktop Manager");
	QApplication::setApplicationVersion("0.6.3-dev");	
	QApplication::setOrganizationDomain("redisdesktop.com");

	MainWin w;
	w.show();
	return a.exec();
}

