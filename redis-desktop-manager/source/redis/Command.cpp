#include "Command.h"

Command::Command()
	: owner(nullptr), commandString(""), dbIndex(-1)
{
	
}

Command::Command(const QString& cmdString, QObject * owner, int db)
	: owner(owner), commandString(cmdString), dbIndex(db)
{
}

Command::Command(const QString& cmdString, QObject * owner, const QString& invokeMethod, int db)
	: owner(owner), commandString(cmdString), dbIndex(db), callBackMethod(invokeMethod)
{

}

bool Command::hasCallback()
{
	return !callBackMethod.isEmpty();
}

QString Command::getCallbackName()
{
	return callBackMethod;
}

bool Command::hasDbIndex() const
{
	return dbIndex >= 0;
}

int Command::getDbIndex() const
{
	return dbIndex;
}

QString Command::getFormattedString() const
{
	return Command::getFormatted(commandString);
}

QString Command::getRawString() const
{
	return commandString;
}

bool Command::isEmpty() const
{
	return commandString.isEmpty();
}

QObject * Command::getOwner()
{
	return owner;
}

QString Command::getFormatted(const QString& command)
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

