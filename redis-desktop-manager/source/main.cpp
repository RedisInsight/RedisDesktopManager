#include "demo.h"
#include <QApplication>

int main(int argc, char *argv[])
{	
	QApplication a(argc, argv);

	QApplication::setApplicationName("Redis Desktop Manager");
	QApplication::setApplicationVersion("0.5.1");	
	QApplication::setOrganizationDomain("redisdesktop.com");

	MainWin w;
	w.show();
	return a.exec();
}

