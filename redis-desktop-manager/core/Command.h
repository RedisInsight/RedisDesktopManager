#pragma once

#include <QtCore>

namespace RedisClient {

class Command 
{    
public:
    Command();
    Command(const QString& cmdString, QObject * owner = nullptr, int db = -1);
    Command(const QString& cmdString, QObject * owner, const QString& invokeMethod, int db = -1);
    Command(const QStringList& cmd, QObject * owner, const QString& invokeMethod, int db = -1);
    Command(const QStringList& cmd, QObject * owner = nullptr, int db = -1);           

    /** @see http://redis.io/topics/protocol for more info **/    
    QByteArray getByteRepresentation() const;
    QString    getRawString() const;
    QString    getCallbackName();
    QString    getProgressCallbackName();
    int        getDbIndex() const;
    QObject *  getOwner() const;

    void setOwner(QObject *);      
    void setCallBackName(const QString &);
    void setProgressCallBackName(const QString &);

    void cancel();

    bool isCanceled() const;
    bool isValid() const;
    bool hasCallback() const;
    bool isEmpty() const;
    bool hasDbIndex() const;
    bool isSelectCommand(int *dbIndex) const;

private:
    QObject * owner;
    QStringList commandWithArguments;
    int dbIndex;
    QString callBackMethod; // method(Response)
    QString progressMethod; // method(unsigned int)
    bool commandCanceled;

    QStringList splitCommandString(const QString &);    

private slots:
    void cancelCommand();
};

}
