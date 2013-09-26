#include "demo.h"
#include <QApplication>

int main(int argc, char *argv[])
{	
	QApplication a(argc, argv);

	QApplication::setApplicationName("Redis Desktop Manager");
	QApplication::setApplicationVersion("0.3.0");	
	QApplication::setOrganizationDomain("glide.name");

	Main w;
	w.show();
	return a.exec();
}
