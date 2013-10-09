#include "..\..\include\redis\Command.h"


QString Command::getFormatted(QString command)
{
	QStringList parts = command.split(" ", QString::SkipEmptyParts, Qt::CaseInsensitive);

	QString formattedCmd("*");
	formattedCmd.append(QString("%1\r\n").arg(parts.length()));

	for (QString part : parts) {
		formattedCmd.append(QString("$%1\r\n%2\r\n")
			.arg(QString("%1").arg(part.length()), part));
	}

	return formattedCmd;
}

