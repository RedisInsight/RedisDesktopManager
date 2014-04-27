#include "Command.h"

Command::Command()
    : owner(nullptr), commandWithArguments(""), dbIndex(-1), commandCanceled(false)
{
    
}

Command::Command(const QString& cmdString, QObject * owner, int db)
    : owner(owner), commandWithArguments(splitCommandString(cmdString)), 
    dbIndex(db), commandCanceled(false)
{
}

Command::Command(const QStringList& cmd, QObject * owner, int db)
    : owner(owner), commandWithArguments(cmd), 
    dbIndex(db), commandCanceled(false)
{
}

Command::Command(const QString& cmdString, QObject * owner, const QString& invokeMethod, int db)
    : owner(owner), commandWithArguments(splitCommandString(cmdString)), 
    dbIndex(db), callBackMethod(invokeMethod), commandCanceled(false)
{

}

Command::Command(const QStringList& cmd, QObject * owner, const QString& invokeMethod, int db)
    : owner(owner), commandWithArguments(cmd), 
    dbIndex(db), callBackMethod(invokeMethod), commandCanceled(false)
{

}

QStringList Command::splitCommandString(const QString &cmd)
{
    //todo implement normal parsing
    return cmd.split(" ", QString::SkipEmptyParts, Qt::CaseInsensitive);
}

bool Command::hasCallback()
{
    return !callBackMethod.isEmpty();
}

QString Command::getCallbackName()
{
    return callBackMethod;
}

void Command::setCallBackName(const QString & name)
{
    callBackMethod = name;
}

bool Command::hasDbIndex() const
{
    return dbIndex >= 0;
}

int Command::getDbIndex() const
{
    return dbIndex;
}

QString Command::getRawString() const
{
    return commandWithArguments.join(' ');
}

bool Command::isEmpty() const
{
    return commandWithArguments.isEmpty();
}

QObject * Command::getOwner() const
{
    return owner;
}

void Command::setOwner(QObject * o)
{
    owner = o;
}

QByteArray Command::getByteRepresentation(const QString& command)
{
   QStringList parts = QStringList();
   int i = 0;
   bool inQuote = false;
   QString part = QString();
   while (i < command.length())
   {
       if(command.at(i).isSpace() && !inQuote)
       {
           if (part.length() > 0)
               parts.append(part);
           part = QString();
       }
       else if (command.at(i) == QChar('"'))
       {
           if (inQuote)
               parts.append(part);
           part = QString();
           inQuote = !inQuote;
       }
       else
       {
           part.append(command.at(i));
       }
       ++i;
   }
   if (parts.length() < 1 || part.length() > 0)
	   parts.append(part);
   return Command::getByteRepresentation(parts);
}

QByteArray Command::getByteRepresentation(const QStringList& command)
{
    QByteArray result;
    result.append(QString("*%1\r\n").arg(command.length()));

    QByteArray partArray;

    for (QString part : command) {
        partArray = part.toUtf8();
        result.append("$");
        result.append(QString::number(partArray.size()));
        result.append("\r\n");
        result.append(partArray);
        result.append("\r\n");
    }

    return result;
}

QByteArray Command::getByteRepresentation() const
{
    return Command::getByteRepresentation(commandWithArguments);
}

void Command::cancel()
{
    commandCanceled = true;
}

bool Command::isCanceled()
{
    return commandCanceled;
}
