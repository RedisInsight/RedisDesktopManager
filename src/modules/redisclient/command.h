#pragma once

#include <QtCore>
#include <functional>

namespace RedisClient {

class Response;

class Command 
{    
public:
    Command();
    Command(const QString& cmdString, QObject * m_owner = nullptr, int db = -1);
    Command(const QStringList& cmd, QObject * m_owner = nullptr, int db = -1);
    Command(const QStringList& cmd, QObject * m_owner, std::function<void(Response)> callback, int db = -1);
    Command(const QStringList& cmd, int db);
    Command(int db);

    Command &operator <<(const QString&);
    Command &append(const QByteArray&part);

    /** @see http://redis.io/topics/protocol for more info **/    
    QByteArray  getByteRepresentation() const;
    QString     getRawString() const;
    QList<QByteArray> getSplitedRepresentattion() const;
    QString     getPartAsString(int i);
    int         getDbIndex() const;
    QObject*    getOwner() const;

    /** New callback API **/
    void setCallBack(QObject* context, std::function<void(Response)> callback);
    std::function<void(Response)> getCallBack();

    void cancel();

    bool isCanceled() const;
    bool isValid() const;
    bool hasCallback() const;
    bool isEmpty() const;
    bool hasDbIndex() const;
    bool isSelectCommand() const;

private:
    QObject * m_owner;
    QList<QByteArray> m_commandWithArguments;
    int dbIndex;
    bool commandCanceled;
    std::function<void(Response)> m_callback;

    QList<QByteArray> splitCommandString(const QString &);
};

QList<QByteArray> convertStringList(const QStringList&list);

}
