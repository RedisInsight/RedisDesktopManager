#include "CrashHandler.h"
#include "demo.h"
#include <QApplication>
#include <QStyleFactory>
#include "version.h"

int main(int argc, char *argv[])
{	
	CrashHandler::instance()->Init(QDir::homePath());

	QApplication a(argc, argv);

	QApplication::setApplicationName("Redis Desktop Manager");
	QApplication::setApplicationVersion(RDM_VERSION);	
	QApplication::setOrganizationDomain("redisdesktop.com");

	MainWin w;
	w.show();
	return a.exec();
}

