#pragma once

#include <QtCore>

#ifndef CALLBACK_MACRO
    #define CALLMETHOD(x) x  //just syntax sugar for clarify purpose of string 
    #define CALLBACK_MACRO
#endif

class Command 
{    
public:
    Command();
    Command(const QString& cmdString, QObject * owner = nullptr, int db = -1);
    Command(const QString& cmdString, QObject * owner, const QString& invokeMethod, int db = -1);
    Command(const QStringList& cmd, QObject * owner, const QString& invokeMethod, int db = -1);
    Command(const QStringList& cmd, QObject * owner = nullptr, int db = -1);

    bool isEmpty() const;

    bool hasDbIndex() const;
    int getDbIndex() const;

    /** @see http://redis.io/topics/protocol for more info **/    
    QByteArray getByteRepresentation() const;
    static QByteArray getByteRepresentation(const QStringList&);
    static QByteArray getByteRepresentation(const QString&);

    QString getRawString() const;
    
    QObject * getOwner() const;
    void setOwner(QObject *);

    bool hasCallback();

    QString getCallbackName();
    void setCallBackName(const QString &);

    void cancel();
    bool isCanceled();

private:
    QObject * owner;
    QStringList commandWithArguments;
    int dbIndex;
    QString callBackMethod;
    bool commandCanceled;

    QStringList splitCommandString(const QString &);

private slots:
    void cancelCommand();
};

